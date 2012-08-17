//
//  sensor.hpp
//  airwave
//
//  Created by Niklas Hauser on 09.06.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef COLOR_CONTROLLER_HPP
#define COLOR_CONTROLLER_HPP

#include "timer_led.hpp"
#include "../common.hpp"

class ColorController
{
public:
	ColorController(volatile uint8_t* red, volatile uint8_t* green, volatile uint8_t* blue)
	:	red(red), green(green), blue(blue)
	{
		fadeToRgbColorValue(0, 0, 0, 0);
	}
	
	ALWAYS_INLINE void
	fadeToRgbColorValue(uint16_t time, uint8_t redValue, uint8_t greenValue, uint8_t blueValue)
	{
		red.fadeTo(time, redValue);
		green.fadeTo(time, greenValue);
		blue.fadeTo(time, blueValue);
	}
	
	ALWAYS_INLINE void
	fadeToRgbColor(uint16_t time, RgbColor rgbColor)
	{
		fadeToRgbColorValue(time, rgbColor.red, rgbColor.green, rgbColor.blue);
	}
	
	ALWAYS_INLINE void
	update()
	{
		red.run();
		green.run();
		blue.run();
	}
	
	inline bool
	isFading()
	{
		return (red.isFading() || green.isFading() || blue.isFading());
	}
	
	ALWAYS_INLINE bool
	turnRedOff()
	{
		return (red.getBrightness() == 0);
	}
	
	ALWAYS_INLINE bool
	turnGreenOff()
	{
		return (green.getBrightness() == 0);
	}
	
	ALWAYS_INLINE bool
	turnBlueOff()
	{
		return (blue.getBrightness() == 0);
	}
	
	RgbColor
	getRgbColor()
	{
		RgbColor rgbColor = {red.getBrightness(), green.getBrightness(), blue.getBrightness()};
		
		return rgbColor;
	}
	
private:
	TimerLed<> red;
	TimerLed<> green;
	TimerLed<> blue;
};

#endif	// COLOR_CONTROLLER_HPP
