/**
 *******************************************************************************
 * @file     SettingsController.c
 * @author   HENIUS (Pawe³ Witak)
 * @version  1.1.1
 * @date     09-11-2012
 * @brief    Obs³uga zapisu ustawieñ do pamiêci EEPROM
 *******************************************************************************
 *
 * <h2><center>COPYRIGHT 2012 HENIUS</center></h2>
 */

/* Sekcja include ------------------------------------------------------------*/

// --->Pliki systemowe

#include <stdint.h>
#include <avr/io.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>

// --->Pliki u¿ytkownika

#include "SettingsController.h"

/* Sekcja zmiennych ----------------------------------------------------------*/

uint8_t *Buffer;					/*!< Bufor do zapisu w pamiêci EEPROM */
uint16_t BufferSize;				/*!< Rozmiar bufora */
uint16_t Index;						/*!< G³ówny indeks przerwania */

/* Sekcja funkcji ------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/**
* @brief    Inicjalizacja kontrolera
* @param    *buffer : wskaŸnik do bufora z danymi
* @param    size : rozmiar bufora
* @retval   Brak
*/
void SettingsCtrl_Init(uint8_t *buffer, uint16_t size)
{
	// Inicjalizacja bufora
	Buffer = buffer;
	BufferSize = size;
	Index = 0;
}

/*----------------------------------------------------------------------------*/
/**
* @brief    Funkcja obs³ugi zapisu danych do pamiêci EEPROM
* @param    Brak
* @retval   Brak
*/
void SettingsCtrl_Handler(void)
{
	if (Index < BufferSize) {
		EECR &= ~_BV(EERIE);
		cli();
		EEAR = SETTINGS_START_ADDR + Index;	// Przygotowanie adresu
		EECR |= _BV(EERE);                  // Rozpoczêcie odczytu
		
		// Wynik odczytu jest gotowy, wiêc nastêpuje porównanie danej do zapisu
		// z dan¹ odczytan¹.
		if (Buffer[Index] != EEDR) { 
			EEAR = SETTINGS_START_ADDR + Index;
			EEDR = Buffer[Index++];	// Bajt do zapisu
			EECR |= _BV(EEMWE);		// Aktywacja EEPROM Master Write
			EECR |= _BV(EEWE);		// Aktywacja EEPROM Write
			EECR |= _BV(EERIE);		// Aktywacja przerwania
		}
		else {
			Index++;
			SettingsCtrl_Handler();
		}	
		
		sei();
	}
	else {
		EECR &= ~_BV(EERIE);		// Wy³¹czenie przerwania
	}	
}

/*----------------------------------------------------------------------------*/
/**
* @brief    Obs³uga przerwania gotowoœci EEPROM'u
* @param    Brak
* @retval   Brak
*/
ISR(EE_READY_vect)
{
	SettingsCtrl_Handler();	
}

/*----------------------------------------------------------------------------*/
/**
* @brief    £adowanie ustawieñ
* @param    Brak
* @retval   Brak
*/
void SettingsCtrl_LoadAll(void)
{
	// Odczyt danych z pamiêci (wszystkich ustawieñ)
	eeprom_read_block(Buffer, SETTINGS_START_ADDR, BufferSize);	
}

/*----------------------------------------------------------------------------*/
/**
* @brief    Zapis ustawieñ
* @param    Brak
* @retval   Brak
*/
void SettingsCtrl_SaveAll(void)
{	
	Index = 0;						// Analiza od pocz¹tku	
	SettingsCtrl_Handler();			// Zapocz¹tkowanie mechanizmu	
}

/******************* (C) COPYRIGHT 2012 HENIUS *************** KONIEC PLIKU ***/