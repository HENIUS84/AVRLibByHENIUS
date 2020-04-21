/**
 *******************************************************************************
 * @file     ADC.c
 * @author   HENIUS (Paweł Witak)
 * @version  1.1.1
 * @date     08-05-2011
 * @brief    Obsługa przetwornika A/C
 *******************************************************************************
 *
 * <h2><center>COPYRIGHT 2011 HENIUS</center></h2>
 */

/* Sekcja include ------------------------------------------------------------*/

// --->Pliki systemowe

#include <stdint.h>
#include <stdio.h>
#include <avr/interrupt.h>

// --->Pliki użytkownika

#include "ADC.h"

/* Sekcja zmiennych ----------------------------------------------------------*/

ADC_t *AdcConfig;					/*!< Wskaźnik do struktury konfiguracji */
volatile uint8_t ChIdx;				/*!< Numer przetwarzanego kanału */

/* Sekcja funkcji ------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/**
 * @brief    Inicjalizacja przetwornika A/C
 * @param    *adcConfig : wskaźnik do struktury konfiguracyjnej
 * @retval   Brak
 */
void ADC_Init(ADC_t *adcConfig)
{
	ADCresolution_t resolution;		// Rozdzielczość przetwornika
	
	AdcConfig = adcConfig;			// Zapamiętanie konfiguracji
	ChIdx = 0;

	// Dostosowanie rozdzielczości pomiaru
	resolution = AdcConfig->Resolution;
	if (resolution == ADCR_11BIT ||
	    resolution == ADCR_12BIT) 
		resolution = ADCR_10BIT;

	ADCSRA = AdcConfig->SampleRate |// Ustawianie szybkości próbkowania
			_BV(ADIE);              // Włączenie przerania
	ADMUX = AdcConfig->Vref       |	// Napięcie referencyjne
			resolution		      |	// Rozdzielczość pomiaru
			AdcConfig->ChannelList[ChIdx]; // Inicjalizacja kanał
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Rozpoczęcie pomiaru
 * @param    Brak
 * @retval   Brak
 */
void ADC_StartConv(void)
{
	ADCSRA |= _BV(ADEN);			// Włączenie przetwornika
	ADCSRA |= _BV(ADSC);			// Rozpoczęcie konwersji
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Przerwanie pomiaru
 * @param    Brak
 * @retval   Brak
 */
void ADC_StopConv(void)
{
	ADCSRA &= ~_BV(ADEN);			// Wyłączenie przetwornika
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Obsługa przerwania na gotowość próbki
 * @param    Brak
 * @retval   Brak
 */
ISR(ADC_vect)
{
	static uint8_t index = 0;		// Indeks pomiaru oversampling'u
	uint8_t oversamplingFactor = 0;	// Współczynnik oversampling'u
		
	// Pobranie wyniku (w zależności od rozdzielczości)
	switch (AdcConfig->Resolution) 
	{
		// 8 bitów
		case ADCR_8BIT:
			AdcConfig->Buffer[ChIdx] = ADCH;
			
			break;
		
		// 10 bitów
		case ADCR_10BIT:
			AdcConfig->Buffer[ChIdx] = ADC;
			
			break;
			
		// Wyższe rozdzielczości - oversampling
		case ADCR_11BIT:
		case ADCR_12BIT:
			oversamplingFactor = 4 << (4 * (AdcConfig->Resolution - 1));
			AdcConfig->OVSbuffer[ChIdx] += ADC;	// Sumowanie aktualnej próbki
			
			// Czy można uśrednić próbki w celu oversamplingu?
			if (index == (oversamplingFactor - 1))
			{			
				AdcConfig->Buffer[ChIdx] = 
					AdcConfig->OVSbuffer[ChIdx] >> AdcConfig->Resolution;					
				AdcConfig->OVSbuffer[ChIdx] = 0;
			}
					
			break;
	}

	// Ustawienie kolejnego kanału
	ChIdx++;
	ChIdx %= AdcConfig->ChannelAmount;
	ADMUX &= ADC_CHANNEL_MASK;
	ADMUX |= AdcConfig->ChannelList[ChIdx];
		
	// Wywoływanie callback'a na zakończenie przetwarzani wszystkich
	// kanałów z listy
	if (!ChIdx && AdcConfig->OnCompleted)
	{
		index++;
		index %= oversamplingFactor;		
		
		if (oversamplingFactor && !index) 
			AdcConfig->OnCompleted();
		else
			AdcConfig->OnCompleted();
	}
	
	// Kolejny pomiar kiedy jest tryb SINGLE, nie przetworzono jeszcze
	// wszystkich kanałów z listy lub kiedy wybrano tryb AUTO albo wybrano
	// oversampling.
	if ((AdcConfig->Mode == ADCM_SINGLE && ChIdx) ||
	     AdcConfig->Mode == ADCM_AUTO ||
	    (oversamplingFactor && index))
		ADC_StartConv();		
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Deinicjalizacja przetwornika A/C
 * @param    Brak
 * @retval   Brak
 */
void ADC_Deinit(void)
{
	ChIdx = 0;						// Reset indeksu kanału ADC
	ADCSRA &= ~(_BV(ADEN) |			// Wyłączanie przetwornika
			    _BV(ADIE));			// Wyłączenie przerwań
}

/******************* (C) COPYRIGHT 2011 HENIUS *************** KONIEC PLIKU ***/