/**
 *******************************************************************************
 * @file     Utils.h                                                   
 * @author   HENIUS (Pawe³ Witak)                                      
 * @version  1.1.1                                                         
 * @date     21-11-2012                                                       
 * @brief    Library with useful functions (header file)
 *******************************************************************************
 *
 * <h2><center>COPYRIGHT 2012 HENIUS</center></h2>
 */

#ifndef UTILS_H_
#define UTILS_H_

/* Include section -----------------------------------------------------------*/

// --->System files

#include <stdbool.h>
#include <stdint.h>

/* Macros, constants and definitions section ---------------------------------*/

// --->Macros

/*! Gets the size of structure member */
#define MEMBER_SIZE(type, member)	(sizeof(((type *)0)->member))
/*! Limiting the variable value */
#define LIMIT_VALUE(val, max, min) \
if ((int16_t)val >= max) \
{ \
	val = max; \
}else if ((int16_t)val < min) \
{ \
	val = min; \
} \

/* Declaration section -------------------------------------------------------*/

//--->Functions

/*----------------------------------------------------------------------------*/
/**
* @brief    Calculates the  8 bit checksum.
* @param    *dataIn : dane wejœcioweinput data
* @param    size : size of input data
* @retval   Calculated checksum
*/
uint8_t CRC8(uint8_t *dataIn, uint16_t size);

/*----------------------------------------------------------------------------*/
/**
* @brief    Converts byte to the ASCII HEX
* @param    result : ASCI HEX value pointer
* @param    byte : converted byte
* @retval   None
*/
void ByteToAsciiHex(uint8_t* result, uint8_t byte);

/*----------------------------------------------------------------------------*/
/**
* @brief    Converts bytes to the ASCII HEX
* @param    result : ASCI HEX value pointer
* @param    byte : converted byte
* @param    length : length of input table
* @retval   None
*/
void BytesToAsciiHex(uint8_t* result, uint8_t* bytes, uint8_t length);

/*----------------------------------------------------------------------------*/
/**
* @brief    Converts the HEX value (half-byte) to the byte
* @param    hex : HEX value
* @retval   Byte
*/
uint8_t HexToByte(uint8_t hex);

/*----------------------------------------------------------------------------*/
/**
* @brief    Converts ASCII HEX value to the byte
* @param    asciHex : ASCII HEX value
* @retval   Byte
*/
uint8_t AsciiHexToByte(uint8_t* asciHex);

#endif 										/* UTILS_H_ */

/******************* (C) COPYRIGHT 2012 HENIUS *************** END OF FILE ****/