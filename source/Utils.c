/**
 *******************************************************************************
 * @file     Utils.h                                                   
 * @author   HENIUS (Pawe³ Witak)                                      
 * @version  1.1.1                                                         
 * @date     29-01-2013                                                       
 * @brief    Biblioteka z u¿ytecznymi funkcjami
 *******************************************************************************
 *
 * <h2><center>COPYRIGHT 2013 HENIUS</center></h2>
 */

/* Sekcja include ------------------------------------------------------------*/

// --->Pliki systemowe

#include <stdio.h>
#include <ctype.h>

// --->Pliki u¿ytkownika

#include "Utils.h"

/* Sekcja zmiennych ----------------------------------------------------------*/

/* Sekcja funkcji ------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/**
* @brief    Obliczanie 8 bitowej sumy kontrolnej
* @param    *dataIn : dane wejœciowe
* @param    size : liczba bajtów danych wejœciowych
* @retval   Brak
*/
uint8_t CRC8(uint8_t *dataIn, uint16_t size)
{
	#define CRC8INIT  0x00
	#define CRC8POLY  0x18          //0x18 = X^8+X^5+X^4+X^0
	
	uint8_t  crc;
	uint16_t loopCount;
	uint8_t  bitCounter;
	uint8_t  data;
	uint8_t  feedbackBit;
	
	crc = CRC8INIT;

	for (loopCount = 0; loopCount != size; loopCount++)
	{
		data = dataIn[loopCount];
		
		bitCounter = 8;
		do 
		{
			feedbackBit = (crc ^ data) & 0x01;
			
			if ( feedbackBit == 0x01 ) {
				crc = crc ^ CRC8POLY;
			}
			crc = (crc >> 1) & 0x7F;
			if ( feedbackBit == 0x01 ) {
				crc = crc | 0x80;
			}
			
			data = data >> 1;
			bitCounter--;
			
		} while (bitCounter > 0);
	}
	
	return crc;
}

/*----------------------------------------------------------------------------*/
/**
* @brief    Konwersja wartoœci HEX (pó³bajta) do postaci bajtu
* @param    hex : wartoœæ HEX
* @retval   Bajt
*/
uint8_t HexToByte(uint8_t hex)
{
	uint8_t result = 0;				// Wynik operacji
	
	// Czy to cyfra?
	if (hex >= '0' && hex <= '9')
	{
		result = hex - '0';
	} else
	// Czy to ma³a litera?
	if (hex >= 'a' && hex <= 'f')
	{
		result = hex - 'a' + 0x0A;
	} else
	// Czy to du¿a litera?
	if (hex >= 'A' && hex <= 'F')
	{
		result = hex - 'A' + 0x0A;
	}
	
	return result;
}

/*----------------------------------------------------------------------------*/
/**
* @brief    Konwersja wartoœci ASCII HEX do postaci bajtu
* @param    asciHex : wartoœæ ASCII HEX
* @retval   Bajt
*/
uint8_t AsciiHexToByte(uint8_t* asciHex)
{
	uint8_t result = 0;				// Wynik operacji
	
	result = HexToByte(asciHex[0]);
	result <<= 4;
	result |= HexToByte(asciHex[1]);
	
	return result;
}

/*----------------------------------------------------------------------------*/
/**
* @brief    Konwersja bajtu do postaci ASCII HEX
* @param    result : tablica z bajtami po konwersji
* @param    byte : konwertowany bajt
* @retval   Brak
*/
void ByteToAsciiHex(uint8_t* result, uint8_t byte)
{
	if (result)
	{		
		// Konwersja na ASCII HEX
		sprintf((char*)result, "%02X", byte);
	}
}

/*----------------------------------------------------------------------------*/
/**
* @brief    Konwersja tablicy bajtów do postaci ASCII HEX
* @param    result : tablica z bajtami po konwersji
* @param    byte : konwertowany bajt
* @param    length : d³ugoœæ Ÿród³owej tablicy
* @retval   Brak
*/
void BytesToAsciiHex(uint8_t* result, uint8_t* bytes, uint8_t length)
{
	uint8_t index;					// Indeks pomocniczy
	
	if (result && bytes)
	{
		// Konwersja na ASCII HEX
		for (index = 0; index < length; index++)
		{
			ByteToAsciiHex(result, bytes[index]);
			result += 2;
		}
	}
}

/******************* (C) COPYRIGHT 2013 HENIUS *************** KONIEC PLIKU ***/
