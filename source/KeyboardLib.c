/**
 *******************************************************************************
 * @file     KeyboardLib.c
 * @author   HENIUS (Paweł Witak)
 * @version  1.1.2
 * @date     16-04-2014
 * @brief    Obsługa klawiatury
 *******************************************************************************
 *
 * <h2><center>COPYRIGHT 2014 HENIUS</center></h2>
 */

/* Sekcja include ------------------------------------------------------------*/

// --->Pliki systemowe

#include <stdio.h>
#include <math.h>

// --->Pliki użytkownika

#include "KeyboardLib.h"

/* Sekcja zmiennych ----------------------------------------------------------*/

Keyboard_t Keyboard;				/*!< Wskaźnik do opisu klawiatury */
uint8_t PressedKey;					/*!< Kod wciśniętego przycisku */
bool IsKbdLocked = false;			/*!< Flaga oznaczająca blokadę klawiatury */
uint8_t AmountOfButtons;			/*!< Liczba przycisków */

/* Sekcja funkcji ------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/**
 * @brief    Pobieranie czasu repetycji
 * @param    requiredTime : wymagany czas repetycji (w ms)
 * @retval   Ustawiany czas repetycji
 */
uint16_t GetRepetitionTime(uint16_t requiredTime)
{
	double time = round(requiredTime / Keyboard.HandlerTime);
	
	return !time ? Keyboard.HandlerTime : time;
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Funkcja inicjalizacji klawiatury
 * @param    *keyboard : wskażnik do opisu klawiatury
 * @retval   Brak
 */
void InitKeyboard(Keyboard_t *keyboard)
{
	uint8_t idx;					// Indeks pomocniczy
	
	Keyboard = *keyboard;
	
	for (idx = 0; idx < Keyboard.AmountOfButtons; idx++)
	{
		Keyboard.ButtonMap[idx].IsToggled = false;
		Keyboard.ButtonMap[idx].RepetitionTimer =
			GetRepetitionTime(LONG_PRESS_TIME);
	}
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Blokowanie klawiatury
 * @param    isLocked : flaga oznaczająca blokadę klawiatury
 * @retval   Brak
 */
void SetKeyboardLock(bool isLocked)
{
	IsKbdLocked = isLocked;	
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Funkcja obsługi klawiatury
 * @param    Brak
 * @retval   Brak
 */
void KeyboardHandler(void)
{	
	uint16_t idx;					// Indeks pomocniczy	
	uint16_t keyMask = 0;			// Maska przycisków
	EKeyState_t currState;			// Aktualny stan przycisku	
	
	if (!IsKbdLocked)				// Klawiatura odblokowana
	{		
		// Pobieranie stanów przycisków
		if (Keyboard.Handler)
		{
			keyMask = Keyboard.Handler();			
		}	
	
		for (idx = 0; idx < Keyboard.AmountOfButtons; idx++)
		{	
			if (keyMask & Keyboard.ButtonMap[idx].Mask)
			{
				currState = Keyboard.ButtonMap[idx].State;
				
				// Czy to długie wciśnięcie?
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
			 
				// Pobieranie kodu wciśniętego przycisku
				PressedKey = Keyboard.ButtonMap[idx].Code;
			}
			else
			{
				currState = KS_NOT_PRESSED;
				Keyboard.ButtonMap[idx].RepetitionTimer =
					GetRepetitionTime(LONG_PRESS_TIME);
			}
		
			// Czy nastąpiła zmiana stanu wciśnięcia przycisku?
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
 * @brief    Pobieranie wciśniętego przycisku
 * @param    Brak
 * @retval   Kod wciśnięcia przycisku (0 - brak wciśnięcia)
 */
uint8_t GetKey(void)
{	
	uint8_t key = PressedKey;
	
	PressedKey = 0;
	
	return key;
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Sprawdzanie wciśnięcia przycisku z zadanym czasem
 * @param    keyCode : kod przycisku
 * @param    state : poszukiwany stan przycisku
 * @retval   Stan wciśnięcia przycisku (true - wciśnięty)
 */
bool GetKeyState(uint8_t keyCode, EKeyState_t state)
{
	// Flaga oznaczająca wciśnięcie przycisku (według zadanego stanu)
	bool isPressed = false;
	// Indeks pomocniczy
	uint16_t idx;
	
	for (idx = 0; idx < Keyboard.AmountOfButtons; idx++)
	{
		if (Keyboard.ButtonMap[idx].Code == keyCode &&
		    Keyboard.ButtonMap[idx].State == state)
		{
			isPressed = true;
			
			// Jeśli badane jest długie przytrzymanie to po pobraniu stanu jest 
			// on kasowany?
			if (state == KS_LONG_PRESS) {
				Keyboard.ButtonMap[idx].State = KS_HELD;
			}
			
			break;
		}
	}
	
	return isPressed;
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Sprawdzanie wciśnięcia przycisku
 * @param    keyCode : kod przycisku
 * @retval   Stan wciśnięcia przycisku (true - wciśnięty)
 */
bool IsKeyPressed(uint8_t keyCode)
{
	return GetKeyState(keyCode, KS_SHORT_PRESS) | 
	       GetKeyState(keyCode, KS_LONG_PRESS) |
		   GetKeyState(keyCode, KS_HELD);
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Sprawdzanie krótkiego wciśnięcia przycisku
 * @param    keyCode : kod przycisku
 * @retval   Stan wciśnięcia przycisku (true - wciśnięty)
 */
bool IsKeyShortPressed(uint8_t keyCode)
{
	return GetKeyState(keyCode, KS_SHORT_PRESS);
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Sprawdzanie długiego wciśnięcia przycisku
 * @param    keyCode : kod przycisku
 * @retval   Stan wciśnięcia przycisku (true - wciśnięty)
 */
bool IsKeyLongPressed(uint8_t keyCode)
{
	return GetKeyState(keyCode, KS_LONG_PRESS);
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Sprawdzanie zmiany stanu wciśnięcia przycisku
 * @param    keyCode : kod przycisku
 * @retval   Zmiana stanu wciśnięcia przycisku (true - zmiana)
 */
bool IsKeyToggled(uint8_t keyCode)
{
	uint8_t idx;					// Indeks pomocniczy
	bool isToggled = false;			// Flaga oznaczająca zmianę stanu
	
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

/******************* (C) COPYRIGHT 2014 HENIUS ************** KONIEC PLIKU ****/
