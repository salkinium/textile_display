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
			CONTROL = 0x12ff,
		};
	}
	
	namespace group
	{
		enum module
		{
			GROUP0 = 0x1200,
			GROUP1 = 0x1201,
			GROUP2 = 0x1202,
			GROUP3 = 0x1203,
			CONTROL = 0x12ff,
		};
	}
	
	namespace command
	{
		enum color
		{
			SET_COLOR = 'L',
			SWAP_COLOR = 'S',
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
