// coding: utf-8

#ifndef	MODULES_HPP
#define MODULES_HPP

// ids and commands -----------------------------------------------------------
namespace common
{
	namespace id
	{
		enum module
		{
			PIXEL1 = 0x1262,
			PIXEL2 = 0x1258,
			PIXEL3 = 0x1276,
			PIXEL4 = 0x12ab,
			PIXEL5 = 0x12fd,
			CONTROL = 0x1234,
		};
	}
	
	namespace group
	{
		enum module
		{
			GROUP0 = 0x1200,
			GROUP1 = 0x1201,
		};
	}
	
	namespace command
	{
		enum color
		{
			SET_COLOR = 'L',
		};
		
		enum control
		{
			CONFIRMATION = 0xfb,
		};
	}
	
	namespace error
	{
		// all error codes must be higher than 0x20
		enum frame
		{
			
		};
	}
}

struct RgbColor
{
	uint8_t red;
	uint8_t green;
	uint8_t blue;
};

struct FadingColor
{
	uint16_t time;
	RgbColor color;
};

#endif	// MODULES_HPP
