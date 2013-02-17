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
#define TIMER_LED_HPP

#include <stdint.h>
#include <xpcc/driver/ui/led/led.hpp>
#include <xpcc/driver/ui/led/tables.hpp>
#include <xpcc/workflow/periodic_timer.hpp>
#include <xpcc/architecture/driver/accessor/flash.hpp>


/**
 * \brief LED PWM implementation for a timer.
 *
 * \author	Niklas Hauser
 * \ingroup led
 *
 * \tparam	PwmTable		the pwm value look-up table
 * \tparam	PwmTableSize	the size of the look-up table
 */
template<
const uint8_t* PwmTable=xpcc::led::table8_256,
uint8_t const PwmTableSize=256 >
class TimerLed : public xpcc::led::Led
{
private:
	volatile uint8_t* overflow;
	
	uint8_t currentValue;
	
	int16_t deltaValue;
	uint16_t startValue;
	uint8_t endValue;
	uint16_t fadeTime;
	
	xpcc::PeriodicTimer<> timer;
	xpcc::accessor::Flash<uint8_t> table;
	
public:
	TimerLed(volatile uint8_t* overflow);
	
	inline void
	setBrightness(uint8_t brightness);
	
	inline uint8_t
	getBrightness();
	
	inline bool
	isFading();
	
	inline void
	fadeTo(uint16_t time, uint8_t brightness);
	
	inline void
	on(uint16_t time=7);
	
	inline void
	off(uint16_t time=7);
	
	/// must be called at least every ms
	inline void
	run();
};

#include "timer_led_impl.hpp"

#endif	// TIMER_LED_HPP
