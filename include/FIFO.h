/**
 *******************************************************************************
 * @file     FIFO.h
 * @author   HENIUS (Paweł Witak)
 * @version  1.01.001
 * @date     19/03/2011
 * @brief    Obsługa kolejki FIFO (plik nagłówkowy)
 *******************************************************************************
 *
 * <h2><center>COPYRIGHT 2011 HENIUS</center></h2>
 */

#ifndef  FIFO_H
#define  FIFO_H

/* Sekcja include ------------------------------------------------------------*/

// Pliki systemowe
#include <stdbool.h>

/* Sekcja stałych, makr i definicji ------------------------------------------*/

// --->Stałe

// --->Makra

// ---> Typy
/**
 * @brief Konfiguracja kolejki FIFO
 */
typedef struct
{
	uint8_t Count;							/*!< Liczba elementów w kolejce */
	uint8_t Size;							/*!< Rozmiar kolejki */
	bool IsQueueFull;						/*!< Status zapełnienia kolejki */
	uint8_t *Buffer;						/*!< Wskaźnik do bufora kolejki */
}FIFO_t;

/* Sekcja deklaracji ---------------------------------------------------------*/

// Inicjalizacja kolejki
void FIFO_Init(FIFO_t *fifo, uint8_t *buffer, uint8_t size);

// Dodawanie elementu do kolejki
void FIFO_Add(FIFO_t *fifo, uint8_t byte);

// Pobieranie elementu z kolejki
uint8_t FIFO_Get(FIFO_t *fifo);

#endif										/* FIFO_H */

/******************* (C) COPYRIGHT 2011 HENIUS *************** KONIEC PLIKU ***/
