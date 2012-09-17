#include <xpcc/architecture/platform.hpp>
#include "../common.hpp"

// IO #########################################################################
GPIO__OUTPUT(LED, A, 8);
GPIO__INPUT(Button, C, 13);
GPIO__OUTPUT(SYNC, D, 15);
GPIO__OUTPUT(EVENT, D, 14);

using namespace xpcc::stm32;

xpcc::stm32::BufferedUsart1 rprUart(500000, 1);
xpcc::stm32::BufferedUsart2 loggerUart(230400, 5);

#include <xpcc/debug/logger.hpp>
xpcc::IODeviceWrapper<xpcc::stm32::BufferedUsart2> loggerDevice(loggerUart);
// Set all four logger streams to use the UART
xpcc::log::Logger xpcc::log::debug(loggerDevice);
xpcc::log::Logger xpcc::log::info(loggerDevice);
xpcc::log::Logger xpcc::log::warning(loggerDevice);
xpcc::log::Logger xpcc::log::error(loggerDevice);

static bool
initClock()
{
	if (!Clock::enableHse(Clock::HseConfig::HSE_BYPASS)) {
		return false;
	}
	
	Clock::enablePll(Clock::PllSource::PLL_HSE, 25, 336);
	return Clock::switchToPll();
}

void tick()
{
	SYNC::toggle();
}

//#define XPCC_RPR_DEBUG 1
#include <xpcc/driver/connectivity/rpr.hpp>

class Communicator : public xpcc::rpr::Callable
{
public:
	void
	anyMessage(xpcc::rpr::Transmitter& /*node*/, xpcc::rpr::Message */*message*/)
	{
		XPCC_LOG_DEBUG << ".";
	}
};

Communicator comm;

FLASH_STORAGE(xpcc::rpr::Listener listenList[]) =
{
	RPR__LISTEN(xpcc::rpr::MESSAGE_TYPE_ANY, xpcc::rpr::ADDRESS_ANY, xpcc::rpr::COMMAND_ANY, comm, Communicator::anyMessage),
};

xpcc::rpr::Node< xpcc::rpr::Interface< xpcc::stm32::BufferedUsart1 > >
rprNode(xpcc::accessor::asFlash(listenList),
		  sizeof(listenList) / sizeof(xpcc::rpr::Listener));

#include <xpcc/workflow.hpp>
xpcc::PeriodicTimer<> timer(16);

MAIN_FUNCTION
{
	initClock();
	SysTickTimer::enable();
	SysTickTimer::attachInterrupt(tick);

	loggerUart.configurePins(loggerUart.REMAP_PD5_PD6);
	rprUart.configurePins(rprUart.REMAP_PB6_PB7);
	
	XPCC_LOG_INFO << "RESTART!\r" << xpcc::endl;
	
	Button::setInput(xpcc::stm32::PULLUP);
	LED::setOutput(xpcc::gpio::HIGH);
	SYNC::setOutput();
	EVENT::setOutput();
	
	rprNode.setAddress(common::id::CONTROL, common::group::GROUP0);
	FadingColor fade;
	fade.time = 10;
	fade.color.red = 0;
	fade.color.green = 0;
	fade.color.blue = 0;
	
	while (1)
	{
		rprNode.update();
		
		if (timer.isExpired())
		{
			EVENT::toggle();
			
			fade.color.red   -= 1;
			fade.color.green -= 1;
			fade.color.blue  -= 1;
			
			uint8_t buffer[20];
			// PIXEL1
			buffer[0] = fade.color.red,
			buffer[1] = fade.color.green,
			buffer[2] = fade.color.blue,
			// PIXEL2
			buffer[3] = fade.color.red + 10,
			buffer[4] = fade.color.green + 10,
			buffer[5] = fade.color.blue + 10,
			// PIXEL3
			buffer[6] = fade.color.red + 20,
			buffer[7] = fade.color.green + 20,
			buffer[8] = fade.color.blue + 20,
			// PIXEL4
			buffer[9] = fade.color.red + 30,
			buffer[10] = fade.color.green + 30,
			buffer[11] = fade.color.blue + 30,
			// PIXEL5
			buffer[12] = fade.color.red + 40,
			buffer[13] = fade.color.green + 40,
			buffer[14] = fade.color.blue + 40,
			rprNode.multicastMessage(common::group::GROUP1, common::command::SET_COLOR, &buffer, 15);
			
//			static FadingColor fadeTransmit;
//			fadeTransmit.time = 10;
//			fadeTransmit.color.red   = fade.color.red;
//			fadeTransmit.color.green = fade.color.green;
//			fadeTransmit.color.blue  = fade.color.blue;
//			rprNode.unicastMessage(common::id::PIXEL5, common::command::SET_COLOR, &fadeTransmit, 5);
//			fadeTransmit.color.red   += 10;
//			fadeTransmit.color.green += 10;
//			fadeTransmit.color.blue  += 10;
//			rprNode.unicastMessage(common::id::PIXEL4, common::command::SET_COLOR, &fadeTransmit, 5);
//			fadeTransmit.color.red   += 10;
//			fadeTransmit.color.green += 10;
//			fadeTransmit.color.blue  += 10;
//			rprNode.unicastMessage(common::id::PIXEL3, common::command::SET_COLOR, &fadeTransmit, 5);
//			fadeTransmit.color.red   += 10;
//			fadeTransmit.color.green += 10;
//			fadeTransmit.color.blue  += 10;
//			rprNode.unicastMessage(common::id::PIXEL2, common::command::SET_COLOR, &fadeTransmit, 5);
//			fadeTransmit.color.red   += 10;
//			fadeTransmit.color.green += 10;
//			fadeTransmit.color.blue  += 10;
//			rprNode.unicastMessage(common::id::PIXEL1, common::command::SET_COLOR, &fadeTransmit, 5);
		}
	}
}

