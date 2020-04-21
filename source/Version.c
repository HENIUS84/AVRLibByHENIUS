/**
 *******************************************************************************
 * @file     Version.c
 * @author   HENIUS (Paweł Witak) 
 * @version  1.1.2
 * @date     2013-04-18
 * @brief    Obsługa wersji oprogramowania
 *************************************************************************************
 *
 * <h2><center>COPYRIGHT 2013 HENIUS</center></h2>
 */

/* Sekcja include ------------------------------------------------------------*/

// --->Pliki użytkownika

#include "Version.h"

// --->Pliki systemowe

#include <stdlib.h>

/* Sekcja zmiennych ----------------------------------------------------------*/

/* Sekcja funkcji ------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/**
 * @brief    Pobieranie godziny kompilacji
 * @param    Brak
 * @retval   Godzina kompilacji
 */
uint8_t GetCompileHour(void)
{
  return COMPILE_HOUR;
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Pobieranie minut kompilacji
 * @param    Brak
 * @retval   Minuty kompilacji
 */
uint8_t GetCompileMinute(void)
{
  return COMPILE_MINUTE;
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Pobieranie sekund kompilacji
 * @param    Brak
 * @retval   Minuty kompilacji
 */
uint8_t GetCompileSecond(void)
{
  return COMPILE_SECOND;
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Pobieranie roku kompilacji
 * @param    Brak
 * @retval   Rok kompilacji
 */
uint16_t GetCompileYear(void)
{
  return COMPILE_YEAR;
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Pobieranie miesiąca kompilacji
 * @param    Brak
 * @retval   Miesiąc kompilacji
 */
uint8_t GetCompileMonth(void)
{
  return COMPILE_MONTH;
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Pobieranie dnia kompilacji
 * @param    Brak
 * @retval   Dzień kompilacji
 */
uint8_t GetCompileDay(void)
{
  return COMPILE_DAY;
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Pobieranie wersji oprogramowania i daty kompilacji
 * @param    *version : wskaźnik do struktury informacji o oprogramowaniu
 * @param    *versionString : łańcuch tekstowy z wersją programu 
 *           (w formacie Major Version.Minor Version.Revision)
 * @retval   Brak
 */
void GetFirmwareVersion(FirmwareInfo_t *version, uint8_t *versionString)
{
	uint8_t tempBuff[10];			// Bufor tymczasowy
	uint8_t index;					// Indeks
	
	// Pobieranie daty kompilacji
	version->CompileDay = GetCompileDay();
	version->CompileMonth = GetCompileMonth();
	version->CompileYear = GetCompileYear();
	version->CompileHour = GetCompileHour();
	version->CompileMinute = GetCompileMinute();
	
	// Pobieranie wersji oprogramowania
	
	// Główny numer
	index = 0;
	while (*versionString && *versionString != '.')
	{
		tempBuff[index++] = *versionString++;
		tempBuff[index] = 0;
	}	
	version->MajorVersion = atoi((char*)tempBuff);
	
	// Drugorzędny numer
	index = 0;
	versionString++;
	while (*versionString && *versionString != '.')
	{
		tempBuff[index++] = *versionString++;
		tempBuff[index] = 0;
	}
	version->MinorVersion = atoi((char*)tempBuff);
	
	// Numer rewizji
	index = 0;
	versionString++;
	while (*versionString && *versionString != '.')
	{
		tempBuff[index++] = *versionString++;
		tempBuff[index] = 0;
	}
	version->Revision = atoi((char*)tempBuff);
}

/******************* (C) COPYRIGHT 2013 HENIUS *************** KONIEC PLIKU ***/
