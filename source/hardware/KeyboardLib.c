/**
 *******************************************************************************
 * @file     KeyboardLib.c
 * @author   HENIUS (Paweł Witak)
 * @version  1.1.2
 * @date     16-04-2014
 * @brief    Keyboard handler
 *******************************************************************************
 *
 * <h2><center>COPYRIGHT 2014 HENIUS</center></h2>
 */

/* Include section -----------------------------------------------------------*/

// --->System files

#include <stdio.h>
#include <math.h>

// --->User files

#include "KeyboardLib.h"

/* Variable section ----------------------------------------------------------*/

/*! Pointer tot he keyboard descriptor */
static Keyboard_t Keyboard;					
static uint8_t PressedKey;					/*!< Pressed button code */
static bool IsKbdLocked = false;			/*!< Keyboard lock flag */

/* Function section ----------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/**
 * @brief    Gets repetition time.
 * @param    requiredTime : required repetition time (in ms)
 * @retval   Set repetition time
 */
static uint16_t GetRepetitionTime(uint16_t requiredTime)
{
	double time = round(requiredTime / Keyboard.HandlerTime);
	
	return !time ? Keyboard.HandlerTime : time;
}

/*----------------------------------------------------------------------------*/
void InitKeyboard(Keyboard_t *keyboard)
{
	uint8_t idx;
	
	Keyboard = *keyboard;
	
	for (idx = 0; idx < Keyboard.AmountOfButtons; idx++)
	{
		Keyboard.ButtonMap[idx].IsToggled = false;
		Keyboard.ButtonMap[idx].RepetitionTimer =
			GetRepetitionTime(LONG_PRESS_TIME);
	}
}

/*----------------------------------------------------------------------------*/
void SetKeyboardLock(bool isLocked)
{
	IsKbdLocked = isLocked;	
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Keyboard handler
 * @param    None
 * @retval   None
 */
void KeyboardHandler(void)
{	
	uint16_t idx;	
	uint16_t keyMask = 0;
	EKeyState_t currState;	
	
	if (!IsKbdLocked)
	{		
		// Key state handling
		if (Keyboard.Handler)
		{
			keyMask = Keyboard.Handler();			
		}	
	
		for (idx = 0; idx < Keyboard.AmountOfButtons; idx++)
		{	
			if (keyMask & Keyboard.ButtonMap[idx].Mask)
			{
				currState = Keyboard.ButtonMap[idx].State;
				
				// Long press
				if (!(--Keyboard.ButtonMap[idx].RepetitionTimer))
				{
					currState = KS_LONG_PRESS; 
					Keyboard.ButtonMap[idx].RepetitionTimer =
						GetRepetitionTime(HOLD_TIME);
				}
				else if (currState == KS_NOT_PRESSED)
				{
					currState = KS_SHORT_PRESS;
				}
			 
				PressedKey = Keyboard.ButtonMap[idx].Code;
			}
			else
			{
				currState = KS_NOT_PRESSED;
				Keyboard.ButtonMap[idx].RepetitionTimer =
					GetRepetitionTime(LONG_PRESS_TIME);
			}
		
			// Button toggling
			if (currState != Keyboard.ButtonMap[idx].State && 
			    currState != KS_LONG_PRESS)
			{
				Keyboard.ButtonMap[idx].IsToggled = true; 
			}
		
			Keyboard.ButtonMap[idx].State = currState;
		}		
	}
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Get pressed key
 * @param    None
 * @retval   Code of pressed key (0 - not pressed)
 */
uint8_t GetKey(void)
{	
	uint8_t key = PressedKey;
	
	PressedKey = 0;
	
	return key;
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Checks if key is pressed in required time.
 * @param    keyCode : code of key
 * @param    state : required key state
 * @retval   State of key press (true - pressed)
 */
static bool GetKeyState(uint8_t keyCode, EKeyState_t state)
{
	bool isPressed = false;
	uint16_t idx;
	
	for (idx = 0; idx < Keyboard.AmountOfButtons; idx++)
	{
		if (Keyboard.ButtonMap[idx].Code == keyCode &&
		    Keyboard.ButtonMap[idx].State == state)
		{
			isPressed = true;
			
			if (state == KS_LONG_PRESS) {
				Keyboard.ButtonMap[idx].State = KS_HELD;
			}
			
			break;
		}
	}
	
	return isPressed;
}

/*----------------------------------------------------------------------------*/
bool IsKeyPressed(uint8_t keyCode)
{
	return GetKeyState(keyCode, KS_SHORT_PRESS) | 
	       GetKeyState(keyCode, KS_LONG_PRESS) |
		   GetKeyState(keyCode, KS_HELD);
}

/*----------------------------------------------------------------------------*/
bool IsKeyShortPressed(uint8_t keyCode)
{
	return GetKeyState(keyCode, KS_SHORT_PRESS);
}

/*----------------------------------------------------------------------------*/
bool IsKeyLongPressed(uint8_t keyCode)
{
	return GetKeyState(keyCode, KS_LONG_PRESS);
}

/*----------------------------------------------------------------------------*/
bool IsKeyToggled(uint8_t keyCode)
{
	uint8_t idx;
	bool isToggled = false;
	
	for (idx = 0; idx < Keyboard.AmountOfButtons; idx++)
	{
		if (Keyboard.ButtonMap[idx].Code == keyCode)
		{
			isToggled = Keyboard.ButtonMap[idx].IsToggled;
			Keyboard.ButtonMap[idx].IsToggled = false;
			
			break;
		}
	}
	
	return isToggled;	
}

/******************* (C) COPYRIGHT 2014 HENIUS *************** END OF FILE ****/
