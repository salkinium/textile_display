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

class RgbLed
{
public:
	RgbLed()
	:	red(&OCR1AL), green(&OCR0A), blue(&OCR0B)
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
		
		if (red.getBrightness() == 0) TCCR1A &= ~(1<<COM1A1);
		else TCCR1A |= (1<<COM1A1);
		
		if (green.getBrightness() == 0) TCCR0A &= ~(1<<COM0A1);
		else TCCR0A |= (1<<COM0A1);
		
		if (blue.getBrightness() == 0) TCCR0A &= ~(1<<COM0B1);
		else TCCR0A |= (1<<COM0B1);
	}
	
	inline bool
	isFading()
	{
		return (red.isFading() || green.isFading() || blue.isFading());
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
