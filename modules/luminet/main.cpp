// coding: utf-8
#include <xpcc/architecture/platform.hpp>

// IO #########################################################################
GPIO__IO(SENSOR1, C, 0);			// ADC0
GPIO__IO(SENSOR2, C, 1);			// ADC1
GPIO__IO(SENSOR3, C, 2);			// ADC2
GPIO__INPUT(LIGHT_SENSOR, C, 3);	// ADC3

GPIO__INPUT(SYNC, D, 2);			// INT0
GPIO__IO(EVENT, D, 3);				// INT1

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

// COMMUNICATION ##############################################################
typedef xpcc::atmega::BufferedUart0 primaryUart;
primaryUart uart(500000);

//#include <xpcc/io/iodevice_wrapper.hpp>
//#include <xpcc/io/iostream.hpp>
//xpcc::IODeviceWrapper< primaryUart > device(uart);
//xpcc::IOStream stream(device);

#include <avr/eeprom.h>
uint8_t volatileGroupPixel;
FadingColor colorBuffer;

#include <xpcc/driver/connectivity/rpr.hpp>

class Communicator : public xpcc::rpr::Callable
{
public:
	void
	setColor(xpcc::rpr::Transmitter& /*node*/, xpcc::rpr::Message *message)
	{
		colorBuffer = *reinterpret_cast<FadingColor *>(message->payload);
	}
	
	void
	setGroupColor(xpcc::rpr::Transmitter& /*node*/, xpcc::rpr::Message *message)
	{
		colorBuffer.time = 10;
		colorBuffer.color.red = *(message->payload + volatileGroupPixel*3);
		colorBuffer.color.green = *(message->payload + volatileGroupPixel*3 + 1);
		colorBuffer.color.blue = *(message->payload + volatileGroupPixel*3 + 2);
	}
	
	void
	swapColorBuffer(xpcc::rpr::Transmitter& /*node*/, xpcc::rpr::Message */*message*/)
	{
		led.fadeToRgbColorValue(colorBuffer.time, colorBuffer.color.red, colorBuffer.color.green, colorBuffer.color.blue);
	}
	
	void
	getID(xpcc::rpr::Transmitter& node, xpcc::rpr::Message *message)
	{
		// colomns, rows, groupsize, groupid
		uint8_t buffer[4] = {8,8,16,volatileGroupPixel};
		node.unicastMessage(message->source, common::command::ANSWER_ID, buffer, 4);
	}
};
Communicator comm;

FLASH_STORAGE(xpcc::rpr::Listener listenList[]) =
{
	RPR__LISTEN(xpcc::rpr::MESSAGE_TYPE_UNICAST, xpcc::rpr::ADDRESS_ANY, common::command::SET_COLOR, comm, Communicator::setColor),
	RPR__LISTEN(xpcc::rpr::MESSAGE_TYPE_MULTICAST, xpcc::rpr::ADDRESS_ANY, common::command::SET_COLOR, comm, Communicator::setGroupColor),
	RPR__LISTEN(xpcc::rpr::MESSAGE_TYPE_BROADCAST, xpcc::rpr::ADDRESS_ANY, common::command::SWAP_COLOR, comm, Communicator::swapColorBuffer),
	RPR__LISTEN(xpcc::rpr::MESSAGE_TYPE_UNICAST, xpcc::rpr::ADDRESS_ANY, common::command::REQUEST_ID, comm, Communicator::getID),
};

xpcc::rpr::Node< xpcc::rpr::Interface< primaryUart > >
rprNode(xpcc::accessor::asFlash(listenList),
		sizeof(listenList) / sizeof(xpcc::rpr::Listener));
// if you change the order of the pointers, you will have to program the eeprom again.
uint16_t EEMEM NonVolatileAddress = 0x1200;
uint16_t EEMEM NonVolatileGroupAddress = common::group::GROUP0;
uint8_t EEMEM NonVolatileGroupPixel = 0;

// INTERRUPTS #################################################################
// SYNC line
ISR(TIMER2_COMPA_vect)
{
	xpcc::Clock::increment();
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
	
	// CTC mode on timer 2
	TCCR2A = (1<<WGM21);
	// 8000kHz / 32 / 250 = 1kHz ~ 1ms
	TCCR2B = (1<<CS21)|(1<<CS20);
	// TOP of 250
	OCR2A = 250;
	// enable compare interrupt
	TIMSK2 = (1<<OCIE2A);
	
	// turn off all LEDs
	BLUE::setOutput(xpcc::gpio::LOW);
	RED::setOutput(xpcc::gpio::LOW);
	GREEN::setOutput(xpcc::gpio::LOW);
	
	SENSOR1::setInput();
	SENSOR2::setInput();
	SENSOR3::setInput();
	LIGHT_SENSOR::setInput();
	
	// set up the oversampling of the photosensitive diode
//	Adc::initialize(xpcc::atmega::Adc::REFERENCE_AREF, xpcc::atmega::Adc::PRESCALER_32);
//	photo::initialize(adcMap, &photoData);
	
	volatileGroupPixel = eeprom_read_byte(&NonVolatileGroupPixel);
	rprNode.setAddress(eeprom_read_word(&NonVolatileAddress), eeprom_read_word(&NonVolatileGroupAddress));
	
	xpcc::atmega::enableInterrupts();
	
	while (1)
	{
		rprNode.update();
		
		led.update();
	}
}
