/**
 *******************************************************************************
 * @file     SettingsController.c
 * @author   HENIUS (Pawe� Witak)
 * @version  1.1.1
 * @date     09-11-2012
 * @brief    Obs�uga zapisu ustawie� do pami�ci EEPROM
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

// --->Pliki u�ytkownika

#include "SettingsController.h"

/* Sekcja zmiennych ----------------------------------------------------------*/

uint8_t *Buffer;					/*!< Bufor do zapisu w pami�ci EEPROM */
uint16_t BufferSize;				/*!< Rozmiar bufora */
uint16_t Index;						/*!< G��wny indeks przerwania */

/* Sekcja funkcji ------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/**
* @brief    Inicjalizacja kontrolera
* @param    *buffer : wska�nik do bufora z danymi
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
* @brief    Funkcja obs�ugi zapisu danych do pami�ci EEPROM
* @param    Brak
* @retval   Brak
*/
void SettingsCtrl_Handler(void)
{
	if (Index < BufferSize) {
		EECR &= ~_BV(EERIE);
		cli();
		EEAR = SETTINGS_START_ADDR + Index;	// Przygotowanie adresu
		EECR |= _BV(EERE);                  // Rozpocz�cie odczytu
		
		// Wynik odczytu jest gotowy, wi�c nast�puje por�wnanie danej do zapisu
		// z dan� odczytan�.
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
		EECR &= ~_BV(EERIE);		// Wy��czenie przerwania
	}	
}

/*----------------------------------------------------------------------------*/
/**
* @brief    Obs�uga przerwania gotowo�ci EEPROM'u
* @param    Brak
* @retval   Brak
*/
ISR(EE_READY_vect)
{
	SettingsCtrl_Handler();	
}

/*----------------------------------------------------------------------------*/
/**
* @brief    �adowanie ustawie�
* @param    Brak
* @retval   Brak
*/
void SettingsCtrl_LoadAll(void)
{
	// Odczyt danych z pami�ci (wszystkich ustawie�)
	eeprom_read_block(Buffer, SETTINGS_START_ADDR, BufferSize);	
}

/*----------------------------------------------------------------------------*/
/**
* @brief    Zapis ustawie�
* @param    Brak
* @retval   Brak
*/
void SettingsCtrl_SaveAll(void)
{	
	Index = 0;						// Analiza od pocz�tku	
	SettingsCtrl_Handler();			// Zapocz�tkowanie mechanizmu	
}

/******************* (C) COPYRIGHT 2012 HENIUS *************** KONIEC PLIKU ***/