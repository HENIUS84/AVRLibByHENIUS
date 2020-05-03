/**
 *******************************************************************************
 * @file     Utils.h                                                   
 * @author   HENIUS (Pawe³ Witak)                                      
 * @version  1.1.1                                                         
 * @date     29-01-2013                                                       
 * @brief    Library with useful functions
 *******************************************************************************
 *
 * <h2><center>COPYRIGHT 2013 HENIUS</center></h2>
 */

/* Include section -----------------------------------------------------------*/

// --->System files

#include <stdio.h>
#include <ctype.h>

// --->User files

#include "Utils.h"

/* Function section ----------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
uint8_t CRC8(uint8_t *dataIn, uint16_t size)
{
	#define CRC8INIT  0x00
	#define CRC8POLY  0x18					//0x18 = X^8+X^5+X^4+X^0
	
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
uint8_t HexToByte(uint8_t hex)
{
	uint8_t result = 0;
	
	if (hex >= '0' && hex <= '9')
	{
		result = hex - '0';
	} else if (hex >= 'a' && hex <= 'f')
	{
		result = hex - 'a' + 0x0A;
	} else if (hex >= 'A' && hex <= 'F')
	{
		result = hex - 'A' + 0x0A;
	}
	
	return result;
}

/*----------------------------------------------------------------------------*/
uint8_t AsciiHexToByte(uint8_t *asciHex)
{
	uint8_t result = 0;
	
	result = HexToByte(asciHex[0]);
	result <<= 4;
	result |= HexToByte(asciHex[1]);
	
	return result;
}

/*----------------------------------------------------------------------------*/
void ByteToAsciiHex(uint8_t *result, uint8_t byte)
{
	if (result)
	{		
		sprintf((char*)result, "%02X", byte);
	}
}

/*----------------------------------------------------------------------------*/
void BytesToAsciiHex(uint8_t* result, uint8_t* bytes, uint8_t length)
{
	uint8_t index;
	
	if (result && bytes)
	{
		for (index = 0; index < length; index++)
		{
			ByteToAsciiHex(result, bytes[index]);
			result += 2;
		}
	}
}

/******************* (C) COPYRIGHT 2013 HENIUS *************** END OF FILE ****/
