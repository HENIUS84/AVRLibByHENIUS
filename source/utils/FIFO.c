/**
 *******************************************************************************
 * @file     FIFO.c
 * @author   HENIUS (Paweł Witak)
 * @version  1.01.001
 * @date     19/03/2011
 * @brief    Obsługa kolejki FIFO
 *******************************************************************************
 *
 * <h2><center>COPYRIGHT 2011 HENIUS</center></h2>
 */

/* Sekcja include ------------------------------------------------------------*/

// Pliki systemowe
#include <stdio.h>
#include <avr/pgmspace.h>
#include <avr/io.h>
#include <stdint.h>
#include <avr/iom128.h>
#include <avr/interrupt.h>

// Pliki użytkownika
#include "FIFO.h"

/* Sekcja zmiennych ----------------------------------------------------------*/
volatile uint16_t I2Ctimeout;       /*!< Licznik timeout'u portu I2C */


/* Sekcja funkcji ------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/**
 * @brief    Inicjalizacja kolejki FIFO
 * @param    *fifo : wskaźnik do konfiguracji kolejki
 * @param    *buffer : wskaźnik do bufora kolejki
 * @param    size : rozmiar kolejki
 * @retval   Brak
 */
void FIFO_Init(FIFO_t *fifo, uint8_t *buffer, uint8_t size)
{
	fifo->Buffer = buffer;				// Inicjalizacja bufora kolejki
	fifo->Size = size;					// Inicjalizacja rozmiaru kolejki
	fifo->Count = 0;					// Resetowanie liczby elementów kolejki
	fifo->IsQueueFull = false;			// Resetowanie stanu kolejki
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Dodawanie elementu do kolejki
 * @param    *fifo : wskaźnik do konfiguracji kolejki
 * @param    byte : dodawany element
 * @retval   Brak
 */
void FIFO_Add(FIFO_t *fifo, uint8_t byte)
{
	// Nastąpi dodanie elementu na koniec kolejki
	if (fifo->Buffer)
	{
		// Czy jest miejsce w kolejce?
		if (fifo->Count < fifo->Size)
		{
			// Jest miejsce, więc można dodać ten element.
			fifo->IsQueueFull = false;
			fifo->Buffer[fifo->Count++] = byte;
		}
		else
		{
			fifo->IsQueueFull = true;	// Kolejka jest pełna
		}
	}
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Pobieranie elementu z kolejki
 * @param    *fifo : wskaźnik do konfiguracji kolejki
 * @retval : Pobrany element (z początku kolejk)
 */
uint8_t FIFO_Get(FIFO_t *fifo)
{
	uint8_t result = 0;				// Pobrany element
	uint8_t index = 0;

	// Nastąpi pobranie elementu z początku kolejki
	if (fifo->Buffer && fifo->Count)
	{
		fifo->Count--;
		fifo->IsQueueFull = false;
		result = fifo->Buffer[0];		// Pobranie pierwszego elementu z kolejki

		// Teraz trzeba przeunąć wszystkie elementy
		for (index = 0; index < fifo->Count; index++)
		{
			fifo->Buffer[index] = fifo->Buffer[index + 1];
		}
	}

	return result;
}

/******************* (C) COPYRIGHT 2011 HENIUS *************** KONIEC PLIKU ***/
