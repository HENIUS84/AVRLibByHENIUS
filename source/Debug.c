/**
 *******************************************************************************
 * @file     Debug.c
 * @author   HENIUS (Paweł Witak)
 * @version  1.1.1
 * @date     18/03/2011
 * @brief    Obsługa trybu debug
 *******************************************************************************
 *
 * <h2><center>COPYRIGHT 2011 HENIUS</center></h2>
 */

/* Sekcja include ------------------------------------------------------------*/

// --->Pliki systemowe

#include <stdint.h>
#include <stdio.h>

// --->Pliki użytkownika

#include "Debug.h"

/* Sekcja zmiennych ----------------------------------------------------------*/

bool DbgModeEnabled = false;		/*!< Aktywacja trybu debug */
uint8_t TextBuffer[100];			/*!< Bufor na tekst */

/* Sekcja funkcji ------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/**
 * @brief    Aktwacja trybu debug
 * @param    isDebugSet : flaga włączająca tryb DEBUG
 * @retval   Brak
 */
void SetDebugMode(bool isDebugSet)
{
	DbgModeEnabled = isDebugSet;
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Wyświetlanie komunikatu debug (z pamięci RAM)
 * @param    *msg : komunikat
 * @param    ... : lista argumentów
 * @retval   Brak
 */
void DebugWrite(char *msg, ...)
{
	va_list args;
	
	if (DbgModeEnabled)
	{
		va_start(args, msg);
		vsprintf((char*)TextBuffer, msg, args); 
		printf("%s", (char*)TextBuffer);
		va_end(args);
	}
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Wyświetlanie komunikatu debug (z pamięci programu)
 * @param    *msg : komunikat
 * @retval   ... : lista argumentów
 */
void DebugWrite_P(char *msg, ...)
{
	va_list args;

	if (DbgModeEnabled)
	{
		va_start(args, msg);
		vsprintf_P((char*)TextBuffer, msg, args);
		printf("%s", TextBuffer);
		va_end(args);
	}
}

/******************* (C) COPYRIGHT 2011 HENIUS *************** KONIEC PLIKU ***/
