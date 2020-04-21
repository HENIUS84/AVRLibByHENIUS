/**
 *******************************************************************************
 * @file     OWICrc.h
 * @author   HENIUS (Paweł Witak)
 * @version  1.01.001
 * @date     13/06/2011
 * @brief    Obliczanie CRC na potrzeby obsługi 1-Wire (na podstawie noty 
             Atmel'a AVR318) (plik nagłówkowy)
 *******************************************************************************
  * 
  * <h2><center>COPYRIGHT 2011 HENIUS</center></h2>
  */

#ifndef OWI_CRC_H
#define OWI_CRC_H

/* Sekcja include ------------------------------------------------------------*/

// Pliki systemowe
#include <stdbool.h>
#include <avr/io.h>

/* Sekcja stałych, makr i definicji ------------------------------------------*/

/* Sekcja deklaracji ---------------------------------------------------------*/

// ---> Funkcje

// Obliczanie CRC8
uint8_t OWI_CRC8(uint8_t inData, uint8_t seed);
// Sprawdzanie CRC z bufora
uint8_t OWI_CalculateCRC(uint8_t *buff, uint8_t length);
// Sprawdzanie poprawności odczytanego adresu
bool OWI_CheckROMCode(uint8_t *romCode);

#endif								/* OWI_CRC_H */

/******************* (C) COPYRIGHT 2011 HENIUS *************** KONIEC PLIKU ***/
