/**
 *******************************************************************************
 * @file     AverageFilters.h
 * @author   HENIUS (Paweł Witak)
 * @version  1.01.001
 * @date     23-06-2011
 * @brief    Implementacja filtrów uśredniających (plik nagłówkowy)
 *******************************************************************************
 *
 * <h2><center>COPYRIGHT 2011 HENIUS</center></h2>
 */

#ifndef  AVERAGE_FILTERS_H_
#define  AVERAGE_FILTERS_H_

/* Sekcja include ------------------------------------------------------------*/

// Pliki systemowe
#include <stdint.h>
#include <stdlib.h>

/* Sekcja stałych, makr i definicji ------------------------------------------*/

// --->Stałe
#define AVG_FILTER_SCALE	(6)		/*!< Współczynnik filtru uśredniającego 2 */
/*! Rozmiar bufora filtru 2 */
#define AVG_FILTER2_SIZE	(1 << AVG_FILTER_SCALE)		
#define AVGF1_SF			(8)		/*!< Współczynnik skalujący filtru 1 */
#define MF_SIZE (5)
// --->Typy
/**
 * @brief Dane konfiguracyjne dla filtru 1
 */
typedef struct  
{
	uint8_t TimeConstant;			/*!< Stała czasowa */
	int32_t PreviousSample;			/*!< Poprzednia próbka */
}AvgFilter1_t;

/**
 * @brief Dane konfiguracyjne dla filtru 2
 */
typedef struct  
{
	uint8_t Index;					/*!< Indeks próbek */	
	int32_t PreviousSum;			/*!< Poprzednia suma próbek */
	/*! Bufor próbek */
	int16_t Buffer[AVG_FILTER2_SIZE];				
}AvgFilter2_t;

/* Sekcja deklaracji ---------------------------------------------------------*/

// --->Funkcje
// Obsługa filtru uśredniającego 1
int32_t AverageFilter1(int32_t currentSample, AvgFilter1_t *filter);
// Obsługa filtru uśredniającego 2
uint16_t AverageFilter2(uint16_t currentSample, AvgFilter2_t *filter);

#endif								/* AVERAGE_FILTERS_H_ */

/******************* (C) COPYRIGHT 2011 HENIUS ************** KONIEC PLIKU ****/
