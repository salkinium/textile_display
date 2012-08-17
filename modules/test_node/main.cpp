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

#define DEBUG 1
#define SYNC_MASTER 0

// COMMUNICATION ##############################################################
typedef xpcc::atmega::BufferedUart0 primaryUart;

#if DEBUG
#include <xpcc/io/iodevice_wrapper.hpp>
#include <xpcc/io/iostream.hpp>
primaryUart uart(115200);
xpcc::IODeviceWrapper< primaryUart > device(uart);
xpcc::IOStream stream(device);
#endif

// COLORS #####################################################################
#include "colorController.hpp"
ColorController color(&OCR1AL, &OCR0A, &OCR0B);

// PHOTOSENSITIVE DIODE #######################################################
typedef xpcc::atmega::AdcInterrupt Adc;
uint8_t adcMapping[1] = {3};
typedef xpcc::atmega::AnalogSensors< Adc,1,4 > photoTransistor;
uint16_t photoSensorData;

// INTERRUPTS #################################################################
// SYNC line
ISR(INT0_vect)
{
	xpcc::Clock::increment();
}

#if SYNC_MASTER
ISR(TIMER2_COMPA_vect)
{
	SENSOR1::toggle();
}
#endif

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
	
#if	SYNC_MASTER
	// CTC Mode on timer 2
	TCCR2A = (1<<WGM21);
	// 8000kHz / 64 / 125 = 1kHz ~ 1ms
	TCCR2B = (1<<CS22);
	OCR2A = 125;
	// enable output compare overflow interrupt
	TIMSK2 = (1<<OCIE2A);
#endif
	
	// enable external interrupts on INT0 and INT1 pins
	EIMSK = (1<<INT0)|(1<<INT1);
	// sensing control on any (SYNC), falling (EVENT) edge of signal
	EICRA = (1<<ISC00)|(1<<ISC11);
	
	// turn off all LEDs
	BLUE::setOutput(xpcc::gpio::LOW);
	RED::setOutput(xpcc::gpio::LOW);
	GREEN::setOutput(xpcc::gpio::LOW);
	
#if	SYNC_MASTER
	SENSOR1::setOutput();
#else
	SENSOR1::setInput();
#endif
	SENSOR2::setInput();
	SENSOR3::setInput();
	LIGHT_SENSOR::setInput();
	
	// set up the oversampling of the photosensitive diode
	Adc::initialize(Adc::REFERENCE_AREF, Adc::PRESCALER_32);
	photoTransistor::initialize(adcMapping, &photoSensorData);
	
	// init is done, full power, Skotty!
	xpcc::atmega::enableInterrupts();
	
#if DEBUG
	const uint16_t fadeTime = 3000;
	const uint8_t table_size = 7;
	uint8_t color_index = 0;
	bool fadeOffNext = false;
	RgbColor color_table[table_size] =
	{
		{255,0,0},
		{0,255,0},
		{0,0,255},
		{255,255,0},
		{0,255,255},
		{255,0,255},
		{255,255,255},
	};
#endif
//	xpcc::PeriodicTimer<> timer(50);
	
	while (1)
	{
#if DEBUG
//		if (photoTransistor::isNewDataAvailable())
//		{
//			RgbColor next = {0,0,photoSensorData>>1};
//			if (photoSensorData > 255) next.blue = 255;
//			color.fadeToRgbColor(0, next);
//		}
//		
//		if (timer.isExpired())
//		{
//			photoTransistor::readSensors();
//		}
		
		if (!color.isFading())
		{
			if (fadeOffNext)
			{
				color.fadeToRgbColorValue(fadeTime, 0,0,0);
			}
			else {
				color.fadeToRgbColor(fadeTime, color_table[color_index++ % table_size]);
			}
			fadeOffNext = !fadeOffNext;
		}
#endif
		color.update();
		
		if (color.turnRedOff()) TCCR1A &= ~(1<<COM1A1);
		else TCCR1A |= (1<<COM1A1);
		
		if (color.turnGreenOff()) TCCR0A &= ~(1<<COM0A1);
		else TCCR0A |= (1<<COM0A1);
			
		if (color.turnBlueOff()) TCCR0A &= ~(1<<COM0B1);
		else TCCR0A |= (1<<COM0B1);
	}
}
