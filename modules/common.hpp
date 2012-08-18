// coding: utf-8

#ifndef	MODULES_HPP
#define MODULES_HPP

// ids and commands -----------------------------------------------------------
namespace comm
{
	namespace id
	{
		enum module
		{
			FRAME0,
			FRAME1,
			FRAME2,
			FRAME3,
			FRAME4,
			FRAME5,
			FRAME6,
			FRAME7,
			FRAME8,
			FRAME9,
			
			RFID = 20,
			ETHERNET = 21,
			DOOR = 22,
		};
		
		enum sensor
		{
			SENSOR0,
			SENSOR1,
			SENSOR2,
			SENSOR3,
			SENSOR4,
			SENSOR5,
			SENSOR6,
			SENSOR7,
			SENSOR8,
			SENSOR9,
			SENSOR10,
			SENSOR11,
			SENSOR12,
			SENSOR13,
			SENSOR14,
			SENSOR15,
			
			SENSOR = 0,
		};
	}
	
	namespace command
	{
		// SAB domain, must all be unique
		enum sensor
		{
			GET_BOTTLE_STATUS = 'B',
			GET_BOTTLE_VALUE = 'V',
			SET_BOTTLE_THRESHOLD = 'T',
			SET_SENSOR_CALIBRATION = 'C',
			GET_SENSOR_CALIBRATION = 'G',
			
			FADE_TO_RGB_COLOR = 'F',
			GET_COLOR_STATUS = 'S',
		};
		
		// AMNB domain, must all be unique
		enum rfid
		{
			GET_RFID_STATUS = 'R',
			GET_DOOR_STATUS = 'D',
		};
		
		enum frame
		{
			GET_ALL_BOTTLE_STATUS = 'G',
			SET_ACTION_OVERRIDE = 'O',
			SET_OVERRIDE_RELEASE = 'E',
			SET_OVERRIDE_RELEASE_ALL = 'P',
			SET_TRANSACTION_COMPLETE = 'A',
			SET_COLOR_OVERRIDE = 'X',
			SET_CALIBRATE_SENSOR = 'F',
			SET_COLOR_FOR_ID = 'L',
			SET_ACTION_FOR_ID = 'T',
			SET_NUMBER_OF_ACTIONS = 'N',
			SET_CALIBRATE_ALL_SENSORS = 'C',
		};
	}
	
	namespace error
	{
		// all error codes must be higher than 0x20
		enum frame
		{
			SENSOR_ID_DOES_NOT_EXIST = 0x45,
			COLOR_ID_DOES_NOT_EXIST = 0x47,
			ACTION_ID_DOES_NOT_EXIST = 0x49,
			ACTION_ID_TOO_HIGH = 0x51,
		};
	}
}

//*
// basic enums ----------------------------------------------------------------
enum DoorStatus
{
	DOOR_CLOSED,
	DOOR_OPEN,
	DOOR_NA,	// any door status
};

enum BottleStatus
{
	NO_BOTTLE,
	FULL_BOTTLE,
	ERROR_BOTTLE,
	BOTTLE_NA,	// any bottle status
};

enum BottleTransition
{
	BOTTLE_NO_CHANGE,
	BOTTLE_FILLED,
	BOTTLE_TAKEN,
	BOTTLE_ERROR,
};

// Colors ---------------------------------------------------------------------
enum ColorId
{
	COLOR_NO_COLOR,
	COLOR_WHITE,
	COLOR_RED,
	COLOR_GREEN,
	COLOR_BLUE,
	COLOR_YELLOW,
	COLOR_BLUE_WHITE,
	COLOR_RED_WHITE,
	COLOR_WHITE_HALF,
	
	// reserve the EEPROM
	NUMBER_OF_COLORS = 50
};

enum ColorTransition
{
	COLOR_TRANSITION_FADE,
	COLOR_TRANSITION_ON_OFF_FADE,
	COLOR_TRANSITION_ON_OFF_FADE_SECONDS,
	COLOR_TRANSITION_ON_OFF_PULSE,	// not implemented yet
	COLOR_TRANSITION_ON_OFF_FLASH,	// not implemented yet
	COLOR_TRANSITION_PULSING,
	COLOR_TRANSITION_FLASHING,		// not implemented yet
};

struct ColorBehavior
{
	ColorTransition transition;
	uint16_t time;
};

struct RgbColor
{
	uint8_t red;
	uint8_t green;
	uint8_t blue;
};

// Actions --------------------------------------------------------------------
enum ActionId
{
	ACTION_NO_BOTTLE,
	ACTION_FULL_BOTTLE,
	ACTION_ERROR_BOTTLE,
	
	ACTION_BOTTLE_TAKEN,
	ACTION_BOTTLE_FILLED,
	
	ACTION_TRANSACTION_COMPLETE_WARM_BOTTLE,
	ACTION_WARM_BOTTLE,
	
	ACTION_TRANSACTION_COMPLETE_NO_BOTTLE,
	
	// little hack
	NUMBER_OF_USED_ACTIONS,
	
	// reserve the EEPROM
	NUMBER_OF_ACTIONS = 50,
	// use this to reload the pattern
	ACTION_UPDATE_PATTERN = 0xff,
};

// Action struct --------------------------------------------------------------
struct Action
{
	// IF MATCH PATTERN:
	// transition from door status
	DoorStatus previousDoor;
	// to door status
	DoorStatus currentDoor;
	// transition from bottle status
	BottleStatus previousBottle;
	// to bottle status
	BottleStatus currentBottle;
	
	// THEN DO:
	// with color id
	ColorId color;
	// and color behavior
	ColorBehavior behavior;
	
	// if true then an external source will trigger the next action
	bool requiresExternalCompletion;
	// next action, can be used to build chains of actions
	ActionId nextAction;
	// what bottle transaction should be reported
	BottleTransition bottleTransition;
};

//*/
#endif	// MODULES_HPP
