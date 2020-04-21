/**
 *******************************************************************************
 * @file     AverageFilters.c
 * @author   HENIUS (Paweł Witak)
 * @version  1.1.1
 * @date     23-06-2011
 * @brief    Implementacja filtrów uśredniających
 *******************************************************************************
 *
 * <h2><center>COPYRIGHT 2011 HENIUS</center></h2>
 */

/* Sekcja include ------------------------------------------------------------*/

// --->Pliki systemowe

#include <stdio.h>
#include <stdbool.h>
#include <math.h>

// --->Pliki użytkownika

#include "AverageFilters.h"

/* Sekcja zmiennych ----------------------------------------------------------*/

/* Sekcja funkcji ------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/**
 * @brief    Obsługa filtru uśredniającego 1
 * @param    currentSample : aktualna próbka
 * @param    *filter : wskaźnik do struktury konfiguracyjnej filtru
 * @retval   Średnia ze wszystkich próbek
 */
int32_t AverageFilter1(int32_t currentSample, AvgFilter1_t *filter)
{
	int32_t result;
		
	result = (((currentSample << AVGF1_SF) >> filter->TimeConstant) + 
	           (filter->PreviousSample << AVGF1_SF) - 
	          ((filter->PreviousSample << AVGF1_SF) >> filter->TimeConstant));
	result >>= AVGF1_SF;
	filter->PreviousSample = currentSample;
			
	return result;
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Obsługa filtru uśredniającego 2
 * @param    currentSample : aktualna próbka
 * @param    *filter : wskaźnik do struktury konfiguracyjnej filtru
 * @retval   Średnia ze wszystkich próbek
 */
uint16_t AverageFilter2(uint16_t currentSample, AvgFilter2_t *filter)
{
    // Liczenie  średniej
	filter->PreviousSum = filter->PreviousSum + (int32_t)currentSample - 
	                      filter->Buffer[filter->Index];
						  
	// Wprowadzenie próbki do bufora
	filter->Buffer[filter->Index++] = currentSample;		
	filter->Index %= AVG_FILTER2_SIZE;
  
	return filter->PreviousSum  >> AVG_FILTER_SCALE;					   
}

/******************* (C) COPYRIGHT 2011 HENIUS *************** KONIEC PLIKU ***/
