/**
 *******************************************************************************
 * @file     OWICrc.c
 * @author   HENIUS (Paweł Witak)
 * @version  1.1.1
 * @date     12/06/2011
 * @brief    1-Wire CRC calculation (based on Atmel'a AVR318 note)
 *******************************************************************************
 *
 * <h2><center>COPYRIGHT 2011 HENIUS</center></h2>
 */

/* Include section -----------------------------------------------------------*/

// --->System files

#include <stdint.h>
#include <stdbool.h>

// --->User files

#include "OWICrc.h"

/* Function section ----------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
uint8_t OWI_CRC8(uint8_t inData, uint8_t seed)
{
    uint8_t bitsLeft;
    uint8_t temp;

    for (bitsLeft = 8; bitsLeft > 0; bitsLeft--)
    {
        temp = ((seed ^ inData) & 0x01);
        if (temp == 0)
        {
            seed >>= 1;
        }
        else
        {
            seed ^= 0x18;
            seed >>= 1;
            seed |= 0x80;
        }
        inData >>= 1;
    }
	
    return seed;    
}

/*----------------------------------------------------------------------------*/
uint8_t OWI_CalculateCRC(uint8_t *buff, uint8_t length)
{
    uint8_t i;
    uint8_t crc8 = 0;
    
    for (i = 0; i < length; i++)
    {
        crc8 = OWI_CRC8(*buff, crc8);
        buff++;
    }
		
    return crc8;
}

/*----------------------------------------------------------------------------*/
bool OWI_CheckROMCode(uint8_t *romCode)
{
	return !OWI_CalculateCRC(romCode, 8) ? true : false;
}

/******************* (C) COPYRIGHT 2011 HENIUS *************** END OF FILE ****/
