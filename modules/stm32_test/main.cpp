#include <xpcc/architecture/platform.hpp>
#include "../common.hpp"

// IO #########################################################################
GPIO__OUTPUT(LED, A, 8);
GPIO__INPUT(Button, C, 13);
GPIO__OUTPUT(SYNC, D, 15);

using namespace xpcc::stm32;

xpcc::stm32::BufferedUsart1 rprUart(38400, 1);
xpcc::stm32::BufferedUsart2 loggerUart(115200, 5);

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

uint8_t buffer[20] = "1 Eurojob und mehr.";

#define XPCC_RPR_DEBUG 0
#include <xpcc/driver/connectivity/rpr.hpp>

class Communicator : public xpcc::rpr::Callable
{
public:
	void
	anyMessage(xpcc::rpr::Transmitter& /*node*/, xpcc::rpr::Message *message)
	{
		XPCC_LOG_DEBUG << "message received, command=" << xpcc::hex << message->command << xpcc::ascii << xpcc::endl;
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
xpcc::PeriodicTimer<> timer(210);
xpcc::PeriodicTimer<> timer2(7000);

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
	
	rprNode.setAddress(common::id::CONTROL, common::group::GROUP0);
	FadingColor fade;
	fade.time = 200;
	fade.color.red = 0;
	fade.color.green = 0;
	fade.color.blue = 0;
	
	while (1)
	{
		rprNode.update();
		
		if (timer.isExpired())
		{
			fade.color.red   += 10;
			fade.color.green += 5;
			fade.color.blue  += 2;
			rprNode.unicastMessage(common::id::PIXEL1, common::command::SET_COLOR, &fade, 5);
//			rprNode.multicastMessage(common::group::GROUP1, common::command::SET_COLOR, &fade, 5);
			fade.color.red   += 10;
			fade.color.green += 5;
			fade.color.blue  += 2;
			rprNode.unicastMessage(common::id::PIXEL2, common::command::SET_COLOR, &fade, 5);
			fade.color.red   += 10;
			fade.color.green += 5;
			fade.color.blue  += 2;
			rprNode.unicastMessage(common::id::PIXEL3, common::command::SET_COLOR, &fade, 5);
			fade.color.red   += 10;
			fade.color.green += 5;
			fade.color.blue  += 2;
			rprNode.unicastMessage(common::id::PIXEL4, common::command::SET_COLOR, &fade, 5);
			fade.color.red   += 10;
			fade.color.green += 5;
			fade.color.blue  += 2;
			rprNode.unicastMessage(common::id::PIXEL5, common::command::SET_COLOR, &fade, 5);
		}

//		if (timer2.isExpired())
//		{
//			rprNode.broadcastMessage('C', buffer, 19);
//		}
	}
}

