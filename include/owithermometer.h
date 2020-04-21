/**
 *******************************************************************************
 * @file     OWIThermometermeter.c
 * @author   HENIUS (Paweł Witak)
 * @version  1.01.002
 * @date     10/04/2013
 * @brief    Obsługa czujnika temperatury na 1-Wire (plik nagłówkowy)
 *******************************************************************************
 *
 * <h2><center>COPYRIGHT 2013 HENIUS</center></h2>
 */

#ifndef OWI_THERMOMETER_H_
#define OWI_THERMOMETER_H_

/* Sekcja include ------------------------------------------------------------*/

// Pliki systemowe
#include <stdbool.h>

// Pliki użytkownika
#include "OWIMaster.h"

/* Sekcja stałych, makr i definicji ------------------------------------------*/

// --->Stałe

// Komendy ROM 1-Wire dla czujnika temperatury
#define OWITC_START_CONV   	(0x44)	/*!< Rozpoczęcie konwersji */
#define OWITC_READ_MEM		(0xBE)	/*!< Odczyt pamięci podręcznej (9 bajtów) */
#define OWITC_WRITE_MEM		(0x4E)	/*!< Zapis pamięci czujnika (3 bajty) */
#define OWITC_COPY_MEM		(0x48)	/*!< Kopiowanie konfiguracji do EEPROM'u */
#define OWITC_RECALL_EEPROM	(0xB8)	/*!< Kopiowanie konfiguracji z EEPROM'u */
#define OWITC_READ_POWER	(0xB4)	/*!< Odczyt stanu zasilania */

#define OWI_TEMP_CONV_TIME	(750)	/*!< Czas konwersji temperatury w ms */

// --->Typy

/**
 * @brief Rozdzielczość pomiaru termometru
 */
typedef enum
{
	OWITR_9BIT,						/*!< Rozdzielczość 9 bitowa */
	OWITR_10BIT,					/*!< Rozdzielczość 10 bitowa */
	OWITR_11BIT,					/*!< Rozdzielczość 11 bitowa */
	OWITR_12BIT						/*!< Rozdzielczość 12 bitowa */
}OWITempRes_t;

/**
 * @brief Struktura inicjalizująca termometr
 */
typedef struct
{	
	uint8_t MaxAmountOfDevices;		/*!< Maks. liczba urządzeń do wykrycia */
	uint8_t AmountOfFoundDevices;	/*!< Liczba znalezionych urządzeń */
	uint8_t TaskTime;				/*!< Czas obsługi zadania (w ms) */	
	/*! Wskaźnik do funkcji opóźniającej */
	void(*Delay_ms)(uint16_t delay);	
	OWITempRes_t Resolution;		/*!< Rozdzielczość pomiaru */
	uint16_t RepetitionTime;		/*!< Czas odpytywania czujnika (w ms) */
	OWIDevice_t *Devices;			/*!< Lista urządzeń */
	int16_t *Temperatures;			/*!< Lista temperatur */
}OWIThermoCtrl_t;

/**
 * @brief Struktura opisu konfiguracji czujnika
 */
typedef union
{
	struct
	{
		uint8_t THReg;				/*!< Rejestr TH albo User Byte 1 */
		uint8_t TLReg;				/*!< Rejestr TL albo User Byte 2 */	
		struct 
		{
			uint8_t UnusedBits : 5;	/*!< Nieużywane bity */
			uint8_t Resolution : 2;	/*!< Ustawianie rozdzielczości */
			uint8_t UnusedBit  : 1;	/*!< Nieużywany bit */
		}ConfigReg;					/*!< Rejestr konfiguracji */
	};	
	
	uint8_t All[3];					/*!< Tablica konfiguracji */
}OWIThermoConfig_t;

/**
 * @brief Struktura opisu pamięci czujnika ((*) zależne od danego czujnika)
 */
typedef union
{
	struct
	{
		uint16_t Temperature;		/*!< Temperatura czujnika */
		/*! Rejestr konfiguracyjny z możliwością zapisu do EEPROM'u (*) */
		OWIThermoConfig_t EEPROMSpace;
		uint8_t Reserved;			/*!< Zarezerwowane miejsce */
		uint8_t CountRemain;		/*!< Wartość pozostała licznika */
		uint8_t CountPerDegree;		/*!< Liczba impulsów dla jednego stopnia */
		uint8_t CRC;				/*!< Suma kontrolna */
	};
		
	uint8_t All[9];					/*!< Tablica pamięci czujnika */
}OWIThermoMem_t;

/**
 * @brief Stany maszyny stanów pomiaru temperatury
 */
typedef enum
{
	OWITS_INIT,						/*!< Inicjalizacja */
	OWITS_SKIP_ROM,					/*!< Pomijanie podawania ROM kodu */
	OWITS_START_CONV,				/*!< Żądanie konwersji temperatury */
	OWITS_WAIT,						/*!< Czekanie na zakończenie konwersji */
	OWITS_WAIT_FOR_NEXT, 			/*!< Oczekiwanie na kolejny pomiar */
	OWITS_READ_MEM_REQUEST,			/*!< Żądanie odczytu pamięci podręcznej */
	OWITS_READ_MEM,     			/*!< Odczyt pamięci podręcznej */
	OWITS_MATCH_ROM					/*!< Wysyłanie kodu danego urządzenia */
}EOWIThermoState_t;

/* Sekcja deklaracji ---------------------------------------------------------*/

// --->Zmienne

// Lista kodów rodziny termometrów
extern EOWIFamily_t ThermoDeviceFC[];

// --->Funkcje

// Inicjalizacja czujnika temperatury na 1-Wire
void OWIThermo_Init(OWIThermoCtrl_t *thermometers);
// Funkcja obsługi pomiaru temperatury
void OWIThermo_Handler(void);
// Funkcja zapisu konfiguracji
void OWIThermo_WriteConfig(OWIThermoConfig_t *config, OWIROMCode_t *romId);
// Odczyt pamięci czujnika
bool OWIThermo_ReadMemory(OWIThermoMem_t *memory, OWIROMCode_t *romId);
// Odczyt temperatury danego czujnika
int16_t OWIThermo_ReadTemperature(OWIROMCode_t *romID);

#endif 								/* OWI_THERMOMETER_H_ */

/******************* (C) COPYRIGHT 2013 HENIUS *************** KONIEC PLIKU ***/
