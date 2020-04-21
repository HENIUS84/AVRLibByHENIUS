/**
 *******************************************************************************
 * @file     KeyboardLib.c
 * @author   HENIUS (Paweł Witak)
 * @version  1.01.2
 * @date     16-04-2014
 * @brief    Keyboard handler (plik nagłówkowy)
 *******************************************************************************
 *
 * <h2><center>COPYRIGHT 2014 HENIUS</center></h2>
 */

#ifndef KEYBOARDLIB_H_
#define KEYBOARDLIB_H_

/* Include section -----------------------------------------------------------*/

// --->System files

#include <stdint.h>
#include <stdbool.h>

/* Macros, constants and definitions section ---------------------------------*/

// --->Constants

/*! Repetition time for long press (in ms) */
#define LONG_PRESS_TIME		(500) 	
/*! Repetition time for button holding (in ms) */
#define HOLD_TIME			(500)

// --->Types

/**
 * @brief Button state
 */
typedef enum
{
	KS_NOT_PRESSED,							/*!< Not pressed */
	KS_SHORT_PRESS,							/*!< Short press */
	KS_LONG_PRESS,							/*!< Long press */
	KS_HELD									/*!< Button held */
}EKeyState_t;

/**
 * @brief Button description structure
 */
typedef struct
{
	uint16_t Mask;							/*!< Button mask */
	uint8_t Code;							/*!< Button code */	
	EKeyState_t State;						/*!< Current button state */
	bool IsToggled;							/*!< Button toggle state */
	uint16_t RepetitionTimer;				/*!< Repetition timer */
}Key_t;

/**
 * @brief Keyboard description structure
 */
typedef struct
{	
	uint8_t AmountOfButtons;				/*!< Count of buttons */
	/*! Pointer to the function handling button state */
	uint16_t(*Handler)(void);
	/*! Task interval for keyboard support (in ms) */
	uint16_t HandlerTime;
	Key_t ButtonMap[];						/*!< Buttons map */	
}Keyboard_t;

/* Declaration section -------------------------------------------------------*/

// --->Functions

/*----------------------------------------------------------------------------*/
/**
 * @brief    Keyboard initialization
 * @param    *keyboard : pointer to the keyboard descriptor
 * @retval   None
 */
void InitKeyboard(Keyboard_t *keyboard);

/*----------------------------------------------------------------------------*/
/**
 * @brief    Keyboard handler
 * @param    None
 * @retval   None
 */
void KeyboardHandler(void);

/*----------------------------------------------------------------------------*/
/**
 * @brief    Checks if key is pressed.
 * @param    keyCode : code of key
 * @retval   Key press state (true - pressed)
 */
bool IsKeyPressed(uint8_t keyCode);

/*----------------------------------------------------------------------------*/
/**
 * @brief    Checks if key is short pressed.
 * @param    keyCode : code of key
 * @retval   Key press state (true - pressed)
 */
bool IsKeyShortPressed(uint8_t keyCode);

/*----------------------------------------------------------------------------*/
/**
 * @brief    Checks if key is long pressed.
 * @param    keyCode : code of key
 * @retval   Key press state (true - pressed)
 */
bool IsKeyLongPressed(uint8_t keyCode);

/*----------------------------------------------------------------------------*/
/**
 * @brief    Checks if key state is toggled
 * @param    keyCode : code of key
 * @retval   Key state toggling (true - change)
 */
bool IsKeyToggled(uint8_t keyCode);

/*----------------------------------------------------------------------------*/
/**
 * @brief    Get pressed key
 * @param    None
 * @retval   Code of pressed key (0 - not presed)
 */
uint8_t GetKey(void);

/*----------------------------------------------------------------------------*/
/**
 * @brief    Locks the keyboard
 * @param    isLocked : lock flag
 * @retval   None
 */
void SetKeyboardLock(bool isLocked);

#endif								/* KEYBOARDLIB_H_ */

/******************* (C) COPYRIGHT 2014 HENIUS *************** END OF FILE ****/
