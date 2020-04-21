/**
 *******************************************************************************
 * @file     KeyboardLib.c
 * @author   HENIUS (Paweł Witak)
 * @version  1.01.2
 * @date     16-04-2014
 * @brief    Obsługa klawiatury (plik nagłówkowy)
 *******************************************************************************
 *
 * <h2><center>COPYRIGHT 2014 HENIUS</center></h2>
 */

#ifndef KEYBOARDLIB_H_
#define KEYBOARDLIB_H_

/* Sekcja include ------------------------------------------------------------*/

// Pliki systemowe
#include <stdint.h>
#include <stdbool.h>

/* Sekcja stałych, makr i definicji ------------------------------------------*/

// --->Stałe

/*! Czas repetycji dla długiego wciśnięcia przycisku */
#define LONG_PRESS_TIME		(500) 	
/*! Czas repetycji dla stanu trzymanego przycisku */
#define HOLD_TIME			(500)

// --->Typy

/**
 * @brief Stan przycisku
 */
typedef enum
{
	KS_NOT_PRESSED,					/*!< Nie wciśnięty */
	KS_SHORT_PRESS,					/*!< Krótkie wciśnięcie */
	KS_LONG_PRESS,					/*!< Długie wciśnięcie */
	KS_HELD							/*!< Przycisk trzymany */
}EKeyState_t;

/**
 * @brief Struktura opisująca przycisk
 */
typedef struct
{
	uint16_t Mask;					/*!< Maska przycisku */
	uint8_t Code;					/*!< Kod przycisku */	
	EKeyState_t State;				/*!< Atualny stan przycisku */
	bool IsToggled;					/*!< Zmiana stanu wcisnięcia przycisku */
	uint16_t RepetitionTimer;		/*!< Timer repetycji */
}Key_t;

/**
 * @brief Struktura opisująca klawiaturę
 */
typedef struct
{	
	uint8_t AmountOfButtons;		/*!< Liczba przycisków */
	/*! Wskaźnik do funkcji pobierającej stan przycisków */
	uint16_t(*Handler)(void);
	/*! Czas zadania obsługi klawiatury (w ms) */
	uint16_t HandlerTime;
	Key_t ButtonMap[];				/*!< Mapa przycisków */	
}Keyboard_t;

/* Sekcja deklaracji ---------------------------------------------------------*/

// --->Funkcje

// Funkcja inicjalizacji klawiatury
void InitKeyboard(Keyboard_t *keyboard);
// Funkcja obsługi klawiatury
void KeyboardHandler(void);
// Sprawdzanie wciśnięcia przycisku
bool IsKeyPressed(uint8_t keyCode);
// Sprawdzanie krótkiego wciśnięcia przycisku
bool IsKeyShortPressed(uint8_t keyCode);
// Sprawdzanie długiego wciśnięcia przycisku
bool IsKeyLongPressed(uint8_t keyCode);
// Sprawdzanie zmiany stanu wciśnięcia przycisku
bool IsKeyToggled(uint8_t keyCode);
// Pobieranie wciśniętego przycisku
uint8_t GetKey(void);
// Blokowanie klawiatury
void SetKeyboardLock(bool isLocked);

#endif								/* KEYBOARDLIB_H_ */

/******************* (C) COPYRIGHT 2014 HENIUS ************** KONIEC PLIKU ****/
