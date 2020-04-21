/**
 *******************************************************************************
 * @file     TDA8551.h
 * @author   HENIUS (Paweł Witak)
 * @version  1.01.001
 * @date     30-03-2012
 * @brief    Obsługa wzmacniacza mocy TDA8551 (plik nagłówkowy)
 *******************************************************************************
 *
 * <h2><center>COPYRIGHT 2012 HENIUS</center></h2>
 */

#ifndef TDA8551_H_
#define TDA8551_H_

/* Sekcja include ------------------------------------------------------------*/

// Pliki systemowe
#include <avr/io.h>
#include <stdbool.h>

/* Sekcja stałych, makr i definicji ------------------------------------------*/

// Wyprowadzenia
// Pin zmiany głośności
#define VOLUME_PORT	   		(PORTA) /*!< Rejestr wyjściowy VOLUME */
#define VOLUME_DDR       	(DDRA)  /*!< Rejestr kierunkowy VOLUME */
#define VOLUME_PIN       	(PINA)  /*!< Rejestr wejściowy VOLUME */
#define VOLUME				(_BV(3))/*!< Maska pinu VOLUME */

// Pin trybu pracy
#define AUDIO_MODE_PORT	   	(PORTA) /*!< Rejestr wyjściowy AUDIO_MODE */
#define AUDIO_MODE_DDR      (DDRA)  /*!< Rejestr kierunkowy AUDIO_MODE */
#define AUDIO_MODE_PIN      (PINA)  /*!< Rejestr wejściowy AUDIO_MODE */
#define AUDIO_MODE			(_BV(4))/*!< Maska pinu AUDIO_MODE */

// Makra
// Pin VOLUME
/*! Stan wysokiej impednacji */
#define VOLUME_HIZ()		VOLUME_DDR &= ~VOLUME; \
                            VOLUME_PORT &= ~VOLUME
/*! Stan wysoki */
#define VOLUME_HIGH()		VOLUME_DDR |= VOLUME; \
                            VOLUME_PORT |= VOLUME
/*! Stan niski */
#define VOLUME_LOW()		VOLUME_DDR |= VOLUME; \
                            VOLUME_PORT &=  ~VOLUME
							 
// Pin AUDIO_MODE
/*! Stan wysokiej impednacji */
#define AUDIO_MODE_HIZ()	AUDIO_MODE_DDR &= ~AUDIO_MODE; \
                            AUDIO_MODE_PORT &= ~AUDIO_MODE
/*! Stan wysoki */
#define AUDIO_MODE_HIGH()	AUDIO_MODE_DDR |= AUDIO_MODE; \
                            AUDIO_MODE_PORT |= AUDIO_MODE
/*! Stan niski */
#define AUDIO_MODE_LOW()	AUDIO_MODE_DDR |= AUDIO_MODE; \
                            AUDIO_MODE_PORT &=  ~AUDIO_MODE
/*! Tryb wyciszenia */
#define TDA8551_MUTE()		AUDIO_MODE_HIZ()
/*! Tryb STANDBY */
#define TDA8551_STANDBY()	AUDIO_MODE_HIGH()
/*! Tryb OPERATING */
#define TDA8551_OPERATING()	AUDIO_MODE_LOW()
							 
// Stałe
#define TDA8551_STEPS_COUNT	(64)	/*!< Liczba korków regulacji głośności */
#define TDA8551_DEF_VOLUME	(6)		/*!< Domyślny poziom głośności w % */

/* Sekcja deklaracji ---------------------------------------------------------*/

// --->Funkcje
// Inicjalizacja obsługi wzmacniacza
void TDA8551_Init(void);
// Ustawianie głośności
void TDA8551_SetVolume(uint8_t volume);
// Pobieranie aktualnej głośności
uint8_t TDA8551_GetVolume(void);
// Zwiększanie/zmniejszanie głośności
void TDA8551_VolumeUpDown(bool isUp);

#endif								/* TDA8551_H_ */

/******************* (C) COPYRIGHT 2012 HENIUS *************** KONIEC PLIKU ***/