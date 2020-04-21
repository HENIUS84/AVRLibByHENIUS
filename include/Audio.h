/**
 *******************************************************************************
 * @file     Audio.h
 * @author   HENIUS (Paweł Witak)
 * @version  1.01.001
 * @date     30-03-2012
 * @brief    Obsługa generowania dźwięku (plik nagłówkowy)
 *******************************************************************************
 *
 * <h2><center>COPYRIGHT 2012 HENIUS</center></h2>
 */

#ifndef AUDIO_H_
#define AUDIO_H_

/* Sekcja include ------------------------------------------------------------*/

// Pliki systemowe
#include <avr/io.h>
#include <stdbool.h>

/* Sekcja stałych, makr i definicji ------------------------------------------*/

// Wyjście dźwięku
#define AUDIO_PORT	   		(PORT?) /*!< Rejestr wyjściowy AUDIO */
#define AUDIO_DDR       	(DDR?)  /*!< Rejestr kierunkowy AUDIO */
#define AUDIO_PIN       	(PIN?)  /*!< Rejestr wejściowy AUDIO */
#define AUDIO				(_BV(?))/*!< Maska pinu AUDIO */

#define FREQ_REGISTER		(OCR1A)	/*!< Rejestr częstotliwości */

// Prescaler
#define AUDIO_PRESC_0		(0)		/*!< Brak sygnału dla timera */
#define AUDIO_PRESC_1		(1)     /*!< Prescaler /1 */
#define AUDIO_PRESC_8		(2)     /*!< Prescaler /8 */
#define AUDIO_PRESC_64		(3)     /*!< Prescaler /64 */
#define AUDIO_PRESC_256		(4)     /*!< Prescaler /256 */
#define AUDIO_PRESC_1024	(5)     /*!< Prescaler /1024 */
#define AUDIO_PRESC			(64)	/*!< Dzielnik częstotliwości */
/*! Wartość bitów CS (prescalera) timera audio */
#define AUDIO_PRESC_CS		(AUDIO_PRESC_64)

/* Sekcja deklaracji ---------------------------------------------------------*/

// --->Funkcje
// Inicjalizacja obsługi audio
void Audio_Init(uint32_t cpuFreq);
// Ustawianie częstotliwości
void Audio_SetFreq(uint16_t freq);
// Zatrzymanie generowania dźwięku
void Audio_StopSound(void);

#endif								/* AUDIO_H_ */

/******************* (C) COPYRIGHT 2012 HENIUS *************** KONIEC PLIKU ***/