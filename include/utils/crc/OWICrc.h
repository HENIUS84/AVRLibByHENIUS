/**
 *******************************************************************************
 * @file     OWICrc.h
 * @author   HENIUS (Paweł Witak)
 * @version  1.01.001
 * @date     13/06/2011
 * @brief    1-Wire CRC calculation (based on Atmel'a AVR318 note) (header file)
 *******************************************************************************
  * 
  * <h2><center>COPYRIGHT 2011 HENIUS</center></h2>
  */

#ifndef OWI_CRC_H
#define OWI_CRC_H

/* Include section -----------------------------------------------------------*/

// --->System files

#include <stdbool.h>
#include <avr/io.h>

/* Declaration section -------------------------------------------------------*/

// --->Functions

/*----------------------------------------------------------------------------*/
/**
 * @brief    Calculates CRC8.
 * @param    inData : input byte
 * @param    seed : initial CRC value
 * @retval : Calculated CRC
 */
uint8_t OWI_CRC8(uint8_t inData, uint8_t seed);

/*----------------------------------------------------------------------------*/
/**
 * @brief    Calculated CRC from buffer
 * @param    *buff : pointer to the buffer
 * @param    length : buffer length (last byte is CRC8)
 * @retval   Calculated CRC
 */
uint8_t OWI_CalculateCRC(uint8_t *buff, uint8_t length);

/*----------------------------------------------------------------------------*/
/**
 * @brief    Checks the correctness of ROM code
 * @param    *romCode : ROM Code to be checked
 * @retval   ROM code status (true - Ok)
 */
bool OWI_CheckROMCode(uint8_t *romCode);

#endif								/* OWI_CRC_H */

/******************* (C) COPYRIGHT 2011 HENIUS *************** END OF FILE ****/
