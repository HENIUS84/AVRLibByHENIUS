/**
 *******************************************************************************
 * @file     Utils.h                                                   
 * @author   HENIUS (Pawe³ Witak)                                      
 * @version  1.1.1                                                         
 * @date     21-11-2012                                                       
 * @brief    Biblioteka z u¿ytecznymi funkcjami (plik nag³ówkowy)
 *******************************************************************************
 *
 * <h2><center>COPYRIGHT 2012 HENIUS</center></h2>
 */

#ifndef UTILS_H_
#define UTILS_H_

/* Sekcja include ------------------------------------------------------------*/

// Pliki systemowe
#include <stdbool.h>
#include <stdint.h>

/* Sekcja sta³ych, makr i definicji ------------------------------------------*/

// --->Makra
/*! Pobieranie rozmiaru cz³onka struktury */
#define MEMBER_SIZE(type, member)	(sizeof(((type *)0)->member))
/*! Ograniczenie wartoœci zmiennej */
#define LIMIT_VALUE(val, max, min) \
if ((int16_t)val >= max) \
{ \
	val = max; \
}else if ((int16_t)val < min) \
{ \
	val = min; \
} \

/* Sekcja deklaracji ---------------------------------------------------------*/

//--->Funkcje

// Obliczanie 8 bitowej sumy kontrolnej
uint8_t CRC8(uint8_t *dataIn, uint16_t size);
// Konwersja bajtu do postaci ASCII HEX
void ByteToAsciiHex(uint8_t* result, uint8_t byte);
// Konwersja tablicy bajtów do postaci ASCII HEX
void BytesToAsciiHex(uint8_t* result, uint8_t* bytes, uint8_t length);
// Konwersja wartoœci HEX do postaci bajtu
uint8_t HexToByte(uint8_t hex);
// Konwersja wartoœci ASCII HEX do postaci bajtu
uint8_t AsciiHexToByte(uint8_t* asciHex);

#endif 										/* UTILS_H_ */

/******************* (C) COPYRIGHT 2012 HENIUS *************** KONIEC PLIKU ***/