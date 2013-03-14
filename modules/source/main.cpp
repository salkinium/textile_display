// coding: utf-8
#include <xpcc/architecture/platform.hpp>
#include <xpcc/architecture/driver.hpp>
#include <xpcc/workflow.hpp>
#include <stdlib.h>

#define IS_DREAM_GENERATOR 1

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
xpcc::PeriodicTimer<> photoTimer(200);
uint16_t photoRequestSource = 0;
bool photoSensingInProgress = false;

// COMMUNICATION ##############################################################
typedef xpcc::atmega::BufferedUart0 primaryUart;
primaryUart uart(500000);

//#include <xpcc/io/iodevice_wrapper.hpp>
//#include <xpcc/io/iostream.hpp>
//xpcc::IODeviceWrapper< primaryUart > device(uart);
//xpcc::IOStream stream(device);

#if IS_DREAM_GENERATOR
xpcc::Timeout<> redT(29);
xpcc::Timeout<> greenT(21);
xpcc::Timeout<> blueT(38);
RgbColor fadeC = {25,8,200};
RgbColor counterC = {25,8,200};

xpcc::Timeout<> colorT(3000);
RgbColor maxC = {255,255,255};
RgbColor maxFadeC = {255,255,255};

xpcc::PeriodicTimer<> timer(15);
xpcc::Timeout<> receiverTimeout(15000);
#endif

#include <avr/eeprom.h>
uint8_t volatileGroupPixel;
FadingColor colorBuffer = {0, {0, 0, 0}};

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
		colorBuffer.color.red = *(message->payload + volatileGroupPixel*3);
		colorBuffer.color.green = *(message->payload + volatileGroupPixel*3 + 1);
		colorBuffer.color.blue = *(message->payload + volatileGroupPixel*3 + 2);
#if IS_DREAM_GENERATOR
		receiverTimeout.restart(15000);
#endif
	}
	
	void
	swapColorBuffer(xpcc::rpr::Transmitter& /*node*/, xpcc::rpr::Message */*message*/)
	{
		if (!photoSensingInProgress)
		{
			led.fadeToRgbColorValue(0, colorBuffer.color.red, colorBuffer.color.green, colorBuffer.color.blue);
		}
	}
	
	void
	getID(xpcc::rpr::Transmitter& node, xpcc::rpr::Message *message)
	{
		// colomns, rows, groupsize, groupid
		uint8_t buffer[4] = {8,8,16,volatileGroupPixel};
		node.unicastMessage(message->source, common::command::ANSWER_ID, buffer, 4);
	}
	
	void
	getPhoto(xpcc::rpr::Transmitter& /*node*/, xpcc::rpr::Message *message)
	{
		if (!photoSensingInProgress)
		{
			// turn off LEDs
			led.fadeToRgbColorValue(0, 0, 0, 0);
			photoSensingInProgress = true;
			// start taking a measurement
			photo::readSensors();
			photoRequestSource = message->source;
		}
	}
};
Communicator comm;

FLASH_STORAGE(xpcc::rpr::Listener listenList[]) =
{
	RPR__LISTEN(xpcc::rpr::MESSAGE_TYPE_UNICAST,	xpcc::rpr::ADDRESS_ANY, common::command::SET_COLOR,		comm, Communicator::setColor),
	RPR__LISTEN(xpcc::rpr::MESSAGE_TYPE_MULTICAST,	xpcc::rpr::ADDRESS_ANY, common::command::SET_COLOR,		comm, Communicator::setGroupColor),
	RPR__LISTEN(xpcc::rpr::MESSAGE_TYPE_BROADCAST,	xpcc::rpr::ADDRESS_ANY, common::command::SWAP_COLOR,	comm, Communicator::swapColorBuffer),
	RPR__LISTEN(xpcc::rpr::MESSAGE_TYPE_UNICAST,	xpcc::rpr::ADDRESS_ANY, common::command::REQUEST_ID,	comm, Communicator::getID),
	RPR__LISTEN(xpcc::rpr::MESSAGE_TYPE_UNICAST,	xpcc::rpr::ADDRESS_ANY, common::command::REQUEST_PHOTO,	comm, Communicator::getPhoto),
	RPR__LISTEN(xpcc::rpr::MESSAGE_TYPE_BROADCAST,	xpcc::rpr::ADDRESS_ANY, common::command::REQUEST_PHOTO,	comm, Communicator::getPhoto),
};

xpcc::rpr::Node< xpcc::rpr::Interface< primaryUart > >
rprNode(xpcc::accessor::asFlash(listenList),
		sizeof(listenList) / sizeof(xpcc::rpr::Listener));
// if you change the order of the pointers, you will have to program the eeprom again.
uint16_t EEMEM NonVolatileAddress = 0;
uint16_t EEMEM NonVolatileGroupAddress = common::group::GROUP0;
uint8_t EEMEM NonVolatileGroupPixel = 0;

// SYS TICK ###################################################################
ISR(TIMER2_COMPA_vect)
{
	xpcc::Clock::increment();
}

MAIN_FUNCTION // ##############################################################
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
	Adc::initialize(xpcc::atmega::Adc::REFERENCE_AREF, xpcc::atmega::Adc::PRESCALER_32);
	photo::initialize(adcMap, &photoData);
	
	volatileGroupPixel = eeprom_read_byte(&NonVolatileGroupPixel);
	rprNode.setAddress(eeprom_read_word(&NonVolatileAddress), eeprom_read_word(&NonVolatileGroupAddress));
	
#if IS_DREAM_GENERATOR
	srand(xpcc::atmega::Adc::readChannel(3));
#endif
	
	xpcc::atmega::enableInterrupts();
	
	// small test program, that cycles through the RGB LEDs, so one can spot
	// problematic nodes immidiately, takes about 2200 ms
	xpcc::Timeout<> testT(200);
	uint8_t testStatus = 0;
	
	while (testStatus < 5)
	{
		if (testT.isExpired())
		{
			switch (testStatus)
			{
				case 0:
					led.fadeToRgbColorValue(200,50,0,0);
					testT.restart(500);
					testStatus = 1;
					break;
					
				case 1:
					led.fadeToRgbColorValue(200,0,50,0);
					testT.restart(500);
					testStatus = 2;
					break;
					
				case 2:
					led.fadeToRgbColorValue(200,0,0,50);
					testT.restart(500);
					testStatus = 3;
					break;
					
				case 3:
					led.fadeToRgbColorValue(200,50,50,50);
					testT.restart(500);
					testStatus = 4;
					break;
					
				default:
					led.fadeToRgbColorValue(0,0,0,0);
					testT.stop();
					testStatus = 5;
					break;
			}
		}
//		led.update();
	}
	
	while (1)
	{
		rprNode.update();
		
#if IS_DREAM_GENERATOR
		if (receiverTimeout.isExpired())
		{
			if (colorT.isExpired())
			{
				static uint8_t maxState = 0;
				
				switch (maxState++)
				{
					case 0:
						maxC.red = 254;
						maxC.green = 100;
						maxC.blue = 100;
						break;
						
					case 1:
						maxC.red = 100;
						maxC.green = 254;
						maxC.blue = 100;
						break;
						
					case 2:
						maxC.red = 100;
						maxC.green = 100;
						maxC.blue = 254;
						break;
						
					case 3:
						maxC.red = 150;
						maxC.green = 30;
						maxC.blue = 30;
						break;
						
					case 4:
						maxC.red = 30;
						maxC.green = 150;
						maxC.blue = 30;
						break;
						
					case 5:
						maxC.red = 30;
						maxC.green = 30;
						maxC.blue = 150;
						break;
						
					case 6:
						maxC.red = 200;
						maxC.green = 1;
						maxC.blue = 1;
						break;
						
					case 7:
						maxC.red = 1;
						maxC.green = 200;
						maxC.blue = 1;
						break;
						
					case 8:
						maxC.red = 1;
						maxC.green = 1;
						maxC.blue = 200;
						break;
						
					default:
						maxC.red = 254;
						maxC.green = 254;
						maxC.blue = 254;
						maxState = 0;
						break;
						
				}
				// max about 8 secs
				colorT.restart(5000);
			}
			
			if (redT.isExpired())
			{
				counterC.red -= 1;
				fadeC.red = counterC.red;
				if (counterC.red > maxFadeC.red)
					fadeC.red = maxFadeC.red;
					
					redT.restart(rand()%60);
					}
			if (greenT.isExpired())
			{
				counterC.green -= 1;
				fadeC.green = counterC.green;
				if (counterC.green > maxFadeC.green)
					fadeC.green = maxFadeC.green;
					greenT.restart(rand()%70);
					}
			if (blueT.isExpired())
			{
				counterC.blue -= 1;
				fadeC.blue = counterC.blue;
				if (counterC.blue > maxFadeC.blue)
					fadeC.blue = maxFadeC.blue;
					blueT.restart(rand()%77);
					}
			
			if (timer.isExpired())
			{
				static uint8_t buffer[48*4];
				
				for(uint8_t i=0; i<16*4; i++)
				{
					buffer[i*3] = fadeC.red + (i+32)*8;
					buffer[i*3+1] = fadeC.green + (i+32)*10;
					buffer[i*3+2] = fadeC.blue + (i+32)*12;
				}
				rprNode.multicastMessage(common::group::GROUP0, common::command::SET_COLOR, buffer, 48);
				rprNode.multicastMessage(common::group::GROUP1, common::command::SET_COLOR, buffer+48, 48);
				rprNode.multicastMessage(common::group::GROUP2, common::command::SET_COLOR, buffer+48*2, 48);
				rprNode.multicastMessage(common::group::GROUP3, common::command::SET_COLOR, buffer+48*3, 48);
				rprNode.broadcastMessage(common::command::SWAP_COLOR, 0, 0);
				
				if (maxFadeC.red > maxC.red)
					maxFadeC.red--;
				else if (maxFadeC.red < maxC.red)
					maxFadeC.red++;
				
				if (maxFadeC.green > maxC.green)
					maxFadeC.green--;
				else if (maxFadeC.green < maxC.green)
					maxFadeC.green++;
				
				if (maxFadeC.blue > maxC.blue)
					maxFadeC.blue--;
				else if (maxFadeC.blue < maxC.blue)
					maxFadeC.blue++;
			}
		}
#endif
	}
}
