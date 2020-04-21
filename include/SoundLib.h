/**
 *******************************************************************************
 * @file     SoudLib.h
 * @author   HENIUS (Paweł Witak)
 * @version  1.01.002
 * @date     02-04-2013
 * @brief    Implementacja biblioteki audio (plik nagłówkowy)
 *******************************************************************************
 *
 * <h2><center>COPYRIGHT 2013 HENIUS</center></h2>
 */

#ifndef SOUND_LIB_H_
#define SOUND_LIB_H_

/* Sekcja include ------------------------------------------------------------*/

// Pliki systemowe
#include <stdio.h>
#include <stdbool.h>

/* Sekcja stałych, makr i definicji ------------------------------------------*/

// --->Stałe
#define SL_AVR						/*!< Obsługa biblioteki przez uK AVR */
#define NOTE_DELAY			(10)		/*!< Przerwa w ms między nutami */

// --->Makra
/*! Obliczanie czasu rwania całej nuty */
#define WHOLE_NOTE_DUR(x)	(60000UL / x * 4)

// --->Typy
/**
 * @brief Rodzaj żądania dźwiękowego
 */
typedef enum
{
	SR_BEEP,						/*!< Generowanie krótkiego dźwięku */
	SR_RING,						/*!< Odgrywanie dzwonka */
	SR_NOTE_WAIT,					/*!< Ocyekiwanie na koniec nuty */
	SR_NOTE_DELAY,					/*!< Przerwa między nutami */
	SR_NO_SOUND,					/*!< Stan bez odgrywania dźwięku */
	SR_SUSPENDED,					/*!< Stan uśpienia */
}ESoundRequest_t;

/**
 * @brief Opis nagłówka dzwonka
 */
typedef struct 
{
	uint8_t Duration;				/*!< Domyślny czas trwania */
    uint8_t Octave;					/*!< Domyślna oktawa */
    uint16_t WholeNoteDuration;		/*!< Czas trwania całej nuty */
	uint16_t RingLength;			/*!< Długość dzwonka */
}RTTTLHeader_t;

/**
 * @brief Opis nuty
 */
typedef struct 
{
	uint16_t Duration;				/*!< Czas trwania */
    uint16_t Frequency;				/*!< Częstotliwość nuty */
}Note_t;

/**
 * @brief Struktura inicjalizująca bibliotekę audio
 */
typedef struct
{
	/*! Wskaźnik do funkcji ustawiającej częstotliowść */
	void(*SetFreq)(uint16_t frequency); 
	/*! Wskaźnik do funkcji zatrzymującej odgrywanie dźwięku */
	void(*StopSound)(void);			
}SoundController_t;

/* Sekcja deklaracji ---------------------------------------------------------*/

// --->Funkcje
// Inicjalizacja biblioteki
void InitSoundLib(SoundController_t *soundLib);
// Funkcja obsługi biblioteki dźwiękowej
void SoundLibHandler(void);
// Generowanie dźwięku o zadanej częstotliwości i czasie trwania
void Beep(uint16_t freq, int16_t duration);
// Odgrywanie dzwonka w formacie RTTTL
void PlayRing(uint8_t *ring);
// Zatrzymanie odgrywania dźwięku
void StopSoundPlay(void);

#endif								/* SOUND_LIB_H_ */

/******************* (C) COPYRIGHT 2013 HENIUS *************** KONIEC PLIKU ***/
