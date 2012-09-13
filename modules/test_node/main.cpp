// coding: utf-8
#include <xpcc/architecture/platform.hpp>

// IO #########################################################################
GPIO__IO(SENSOR1, C, 0);			// ADC0
GPIO__IO(SENSOR2, C, 1);			// ADC1
GPIO__IO(SENSOR3, C, 2);			// ADC2
GPIO__INPUT(LIGHT_SENSOR, C, 3);	// ADC3

GPIO__INPUT(SYNC, D, 2);			// INT0
GPIO__IO(EVENT, D, 7);				// INT1

GPIO__OUTPUT(RED, B, 1);			// OCR1AL
GPIO__OUTPUT(GREEN, D, 6);			// OCR0A
GPIO__OUTPUT(BLUE, D, 5);			// OCR0B

// COLORS #####################################################################
#include "rgbLed.hpp"
RgbLed led;

// PHOTOSENSITIVE DIODE #######################################################
typedef xpcc::atmega::AdcInterrupt Adc;
uint8_t adcMap[1] = {3};
typedef xpcc::atmega::AnalogSensors< Adc,1,3 > photo;
uint16_t photoData;

#define DEBUG 0

// COMMUNICATION ##############################################################
typedef xpcc::atmega::BufferedUart0 primaryUart;
primaryUart uart(38400);

#if DEBUG
#include <xpcc/io/iodevice_wrapper.hpp>
#include <xpcc/io/iostream.hpp>
xpcc::IODeviceWrapper< primaryUart > device(uart);
xpcc::IOStream stream(device);
#else
#include <xpcc/driver/connectivity/rpr.hpp>

class Communicator : public xpcc::rpr::Callable
{
public:
	void
	setColor(xpcc::rpr::Transmitter& node, xpcc::rpr::Message *message)
	{
		FadingColor *fade = reinterpret_cast<FadingColor *>(message->payload);
		led.fadeToRgbColorValue(fade->time, fade->color.red, fade->color.green, fade->color.blue);
		
		if (message->type == xpcc::rpr::MESSAGE_TYPE_UNICAST)
			node.unicastMessage(message->source, common::command::CONFIRMATION, 0, 0);
	}
};
Communicator comm;

FLASH_STORAGE(xpcc::rpr::Listener listenList[]) =
{
	RPR__LISTEN(xpcc::rpr::MESSAGE_TYPE_UNICAST, xpcc::rpr::ADDRESS_ANY, xpcc::rpr::COMMAND_ANY, comm, Communicator::setColor),
	RPR__LISTEN(xpcc::rpr::MESSAGE_TYPE_MULTICAST, xpcc::rpr::ADDRESS_ANY, xpcc::rpr::COMMAND_ANY, comm, Communicator::setColor),
};

xpcc::rpr::Node< xpcc::rpr::Interface< primaryUart > >
rprNode(xpcc::accessor::asFlash(listenList),
		sizeof(listenList) / sizeof(xpcc::rpr::Listener));
#endif

// INTERRUPTS #################################################################
// SYNC line
ISR(INT0_vect)
{
	xpcc::Clock::increment();
}

// EVENT line
ISR(INT1_vect)
{
	// update colors now
}

#include <xpcc/architecture/driver.hpp>
#include <xpcc/workflow.hpp>
MAIN_FUNCTION // FINALLY ######################################################
{
	// Fast PWM Mode on timer 0, non-inverting mode
	// set OCnX at BOTTOM, clear OCnX on Compare Match
    TCCR0A = (1<<WGM01)|(1<<WGM00);
	// 8000kHz / 8 / 255 = 3.92kHz ~ 0.255ms
    TCCR0B = (1<<CS01);
	
	// Fast PWM Mode on timer 1, non-inverting mode, restricted to 8 bits
	//set OCnA at BOTTOM, clear OCnA on Compare Match
	TCCR1A = (1<<WGM10);
	// 8000kHz / 8 / 255 = 3.92kHz ~ 0.255ms
	TCCR1B = (1<<WGM12)|(1<<CS11);
	
	// enable external interrupts on INT0 and INT1 pins
	EIMSK = (1<<INT0)|(1<<INT1);
	// sensing control on any (SYNC), falling (EVENT) edge of signal
	EICRA = (1<<ISC00)|(1<<ISC11);
	
	// turn off all LEDs
	BLUE::setOutput(xpcc::gpio::LOW);
	RED::setOutput(xpcc::gpio::LOW);
	GREEN::setOutput(xpcc::gpio::LOW);
	
	SENSOR1::setInput();
	SENSOR2::setInput();
	SENSOR3::setInput();
	LIGHT_SENSOR::setInput();
	
	// set up the oversampling of the photosensitive diode
	Adc::initialize(xpcc::atmega::Adc::REFERENCE_AREF, xpcc::atmega::Adc::PRESCALER_32);
	photo::initialize(adcMap, &photoData);
	
	// init is done, full power, Skotty!
	xpcc::atmega::enableInterrupts();
	
#if DEBUG
	const uint8_t table_size = 8;
	RgbColor color_table[table_size] =
	{
		{255,0,0},
		{0,255,0},
		{0,0,255},
		{255,255,0},
		{0,255,255},
		{255,0,255},
		{255,255,255},
		{127,127,127},
	};
	
	stream << "RESTART" << xpcc::endl;
#else
	rprNode.setAddress(common::id::PIXEL1, common::group::GROUP1);
//	uint8_t buffer[20] = "1 Eurojob und mehr.";
#endif
	xpcc::PeriodicTimer<> timer(17);
	xpcc::PeriodicTimer<> timer3(5000);
	xpcc::PeriodicTimer<> timer2(7000);
	
	while (1)
	{
#if DEBUG
		if (photo::isNewDataAvailable())
		{
			uint8_t value = *photo::getData()>>2;
			led.fadeToRgbColorValue(15, 0, 0, value);
		}

		
		if (timer.isExpired())
		{
			photo::readSensors();
		}
#else
		rprNode.update();
#endif
		led.update();
	}
}
