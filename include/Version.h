/**
 *******************************************************************************
 * @file     Version.h                                                      
 * @author   HENIUS (Paweł Witak)                                              
 * @version  1.01.002                                                          
 * @date     2013-04-18                                             
 * @brief    Obsługa wersji oprogramowania (plik nagłówkowy)                                 
 *******************************************************************************
 *
 * <h2><center>COPYRIGHT 2013 HENIUS</center></h2>
 */

#ifndef VERSION_H_
#define VERSION_H_

/* Sekcja include ------------------------------------------------------------*/

// Pliki systemowe
#include <stdint.h>
#include <stdbool.h>

/* Sekcja stałych, makr i definicji ------------------------------------------*/

// --->Makra obsługujące wersję oprogramowania

/*! Pobieranie godziny kompilacji */
#define COMPILE_HOUR		(((__TIME__[0] - '0') * 10) + \
                               __TIME__[1] - '0')
/*! Pobieranie minut kompilacji */
#define COMPILE_MINUTE		(((__TIME__[3] - '0') * 10) + \
                               __TIME__[4] - '0')
/*! Pobieranie sekund kompilacji */
#define COMPILE_SECOND		(((__TIME__[6] - '0') * 10) + \
                              (__TIME__[7] - '0'))
/*! Pobieranie roku kompilacji */
#define COMPILE_YEAR		((((__DATE__[7] - '0') * 10 + \
                               (__DATE__[8] - '0')) * 10 + \
                               (__DATE__[9] - '0')) * 10 + \
						       (__DATE__[10] - '0'))
/*! Pobieranie miesiąca kompilacji (0-11) */
#define COMPILE_MONTH		(__DATE__[2] == 'n' ? (__DATE__[1] == 'a' ? 1 : 6) \
                           : __DATE__[2] == 'b' ? 2 \
						   : __DATE__[2] == 'r' ? (__DATE__[0] == 'M' ? 3 : 4) \
                           : __DATE__[2] == 'y' ? 5 \
                           : __DATE__[2] == 'l' ? 7 \
                           : __DATE__[2] == 'g' ? 8 \
                           : __DATE__[2] == 'p' ? 9 \
                           : __DATE__[2] == 't' ? 10 \
                           : __DATE__[2] == 'v' ? 11 : 12)
/*! Pobieranie dnia kompilacji */
#define COMPILE_DAY			((__DATE__[4]== ' ' ? 0 : __DATE__[4] - '0') * 10 + \
                             (__DATE__[5] - '0'))
							 
// --->Typy

/**
 * @brief Opis wersji oprogramowania
 */
typedef struct
{
	/*! Dzień kompilacji */
	uint16_t CompileDay    : 5;
	/*! Miesiąc kompilacji */
	uint16_t CompileMonth  : 4;
	/*! Rok kompilacji */
	uint16_t CompileYear   : 12;
	/*! Godzina kompilacji */
	uint16_t CompileHour   : 5;
	/*! Minuty kompilacji */
	uint16_t CompileMinute : 6;
	/*! Główna wersja */
	uint16_t MajorVersion  : 3;
	/*! Druga wersja */
	uint16_t MinorVersion  : 7;
	/* Numer rewizji */
	uint16_t Revision      : 10;
}FirmwareInfo_t;

/* Sekcja deklaracji ---------------------------------------------------------*/

// --->Funkcje

uint8_t GetCompileHour(void);		// Pobieranie godziny kompilacji
uint8_t GetCompileMinute(void);		// Pobieranie minut kompilacji
uint8_t GetCompileSecond(void);		// Pobieranie sekund kompilacji
uint16_t GetCompileYear(void);	    // Pobieranie roku kompilacji
uint8_t GetCompileMonth(void);		// Pobieranie miesiąca kompilacji
uint8_t GetCompileDay(void);		// Pobieranie dnia kompilacji
// Pobieranie wersji oprogramowania i daty kompilacji
void GetFirmwareVersion(FirmwareInfo_t *version, uint8_t *versionString);

#endif								/* VERSION_H_  */

/******************* (C) COPYRIGHT 2013 HENIUS *************** KONIEC PLIKU ***/
