// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2012, Roboterclub Aachen e.V.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Roboterclub Aachen e.V. nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ROBOTERCLUB AACHEN E.V. ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ROBOTERCLUB AACHEN E.V. BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
// ----------------------------------------------------------------------------

#ifndef TIMER_LED_HPP
#	error "Don't include this file directly, use 'timer_led.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
template < const uint8_t* PwmTable,  uint8_t const PwmTableSize >
TimerLed<PwmTable, PwmTableSize>::TimerLed(volatile uint8_t* overflow)
:	overflow(overflow), currentValue(0), deltaValue(0), startValue(0), endValue(0), fadeTime(0), timer(1), table(PwmTable)
{
	*overflow = table[0];
}

template < const uint8_t* PwmTable,  uint8_t const PwmTableSize >
void
TimerLed<PwmTable, PwmTableSize>::setBrightness(uint8_t brightness)
{
//	fadeTime = 0;
	currentValue = brightness;
	*overflow = table[currentValue];
}

template < const uint8_t* PwmTable,  uint8_t const PwmTableSize >
uint8_t
TimerLed<PwmTable, PwmTableSize>::getBrightness()
{
	return currentValue;
}

template < const uint8_t* PwmTable,  uint8_t const PwmTableSize >
bool
TimerLed<PwmTable, PwmTableSize>::isFading()
{
	return static_cast<bool>(fadeTime);
}

template < const uint8_t* PwmTable,  uint8_t const PwmTableSize >
void
TimerLed<PwmTable, PwmTableSize>::fadeTo(uint16_t time, uint8_t brightness)
{
	if (brightness == currentValue) return;
	
	if (!time) {
		currentValue = brightness;
		*overflow = table[currentValue];
	}
	else {
		startValue = static_cast<uint16_t>(currentValue)*100;
		endValue = brightness;
		deltaValue = (endValue - currentValue)*100 / static_cast<int16_t>(time);
	}
	fadeTime = time;
}

template < const uint8_t* PwmTable,  uint8_t const PwmTableSize >
void
TimerLed<PwmTable, PwmTableSize>::on(uint16_t time)
{
	fadeTo(time, PwmTableSize-1);
}

template < const uint8_t* PwmTable,  uint8_t const PwmTableSize >
void
TimerLed<PwmTable, PwmTableSize>::off(uint16_t time)
{
	fadeTo(time, 0);
}

template < const uint8_t* PwmTable,  uint8_t const PwmTableSize >
void
TimerLed<PwmTable, PwmTableSize>::run()
{
	if (timer.isExpired() && fadeTime)
	{
		startValue += deltaValue;
		currentValue = startValue/100;
		if (!--fadeTime) currentValue = endValue;
		
		*overflow = table[currentValue];
	}
}




