/**
 *******************************************************************************
 * @file     Audio.c
 * @author   HENIUS (Paweł Witak)
 * @version  1.1.1
 * @date     30-03-2012
 * @brief    Obsługa generowania dźwięku
 *******************************************************************************
 *
 * <h2><center>COPYRIGHT 2012 HENIUS</center></h2>
 */

/* Sekcja include ------------------------------------------------------------*/

// --->Pliki systemowe

#include <math.h>
#include <avr/io.h>

// --->Pliki użytkownika

#include "Audio.h"

/* Sekcja zmiennych ----------------------------------------------------------*/

uint32_t CPUfrequency;				/*!< Częstotliwość CPU w MHz */

/* Sekcja funkcji ------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/**
 * @brief    Inicjalizacja obsługi audio
 * @param    cpuFreq : częstotliwość CPU w MHz
 * @retval   Brak
 */
void Audio_Init(uint32_t cpuFreq)
{
	CPUfrequency = cpuFreq;
	
	// Konfigurowanie wyprowadzenia
	AUDIO_DDR |= AUDIO;
	
	// Konfiguracja timera
	TCCR1A |= _BV(COM1A0);
	TCCR1B |= _BV(WGM12);	 
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Rozpoczęcie generowania dźwięku
 * @param    Brak
 * @retval   Brak
 */
void Audio_StartSound(void)
{
	TCCR1B |= AUDIO_PRESC_CS << CS00;
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Zatrzymanie generowania dźwięku
 * @param    Brak
 * @retval   Brak
 */
void Audio_StopSound(void)
{
	TCCR1B &= ~AUDIO_PRESC_CS << CS00;
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Ustawianie częstotliwości
 * @param    freq : częstotliwość w Hz
 * @retval   Brak
 */
void Audio_SetFreq(uint16_t freq)
{
	// Ustawienie częstotliwości
	FREQ_REGISTER = CPUfrequency / (2 * (uint32_t)freq * AUDIO_PRESC) - 1;
	// Rozpoczęcie generowania dzwięku
	Audio_StartSound(); 
}	

/******************* (C) COPYRIGHT 2012 HENIUS *************** KONIEC PLIKU ***/
