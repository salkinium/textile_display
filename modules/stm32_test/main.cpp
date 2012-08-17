#include <xpcc/architecture/platform.hpp>

// ----------------------------------------------------------------------------
GPIO__OUTPUT(LED, A, 8);
GPIO__INPUT(Button, C, 13);
GPIO__OUTPUT(SYNC, C, 6);

using namespace xpcc::stm32;

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

#include <xpcc/workflow.hpp>
MAIN_FUNCTION
{
	initClock();
	SysTickTimer::enable();
	SysTickTimer::attachInterrupt(tick);

	Button::setInput(xpcc::stm32::PULLUP);
	LED::setOutput(xpcc::gpio::HIGH);
	SYNC::setOutput();
	xpcc::PeriodicTimer<> timer(1000);
	bool buttonStatus(false);
	
	while (1)
	{
		if (buttonStatus != Button::read())
		{
			buttonStatus = Button::read();
			if (buttonStatus) timer.restart(400);
			else timer.restart(100);
		}
		
		if (timer.isExpired())
		{
			LED::toggle();
		}
	}
}

