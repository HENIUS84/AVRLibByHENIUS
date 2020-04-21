/**
 *******************************************************************************
 * @file     OWIMaster.c
 * @author   HENIUS (Paweł Witak)
 * @version  1.1.1
 * @date     11/06/2011
 * @brief    1-Wire driver in I2C Master driver mode
 *           (based on Atmel'a AVR318 note)
 *******************************************************************************
 *
 * <h2><center>COPYRIGHT 2011 HENIUS</center></h2>
 */

/* Include section -----------------------------------------------------------*/

// --->System files

#include <stdint.h>
#include <stdbool.h>
#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>

// --->User files

#include "OWIMaster.h"
#include "OWICrc.h"

/* Variable section ----------------------------------------------------------*/

uint8_t IntState;							/*!< Interrupt state */

/* Function section ----------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
bool OWIMaster_Init(void)
{
	uint8_t result;
	
	SAVE_INTERRUPT();
	
	// RESET signal
	OWI_PULL_BUS_LOW();
	_delay_us(OWI_DELAY_H);		
		
	// PRESENCE signal
	OWI_RELEASE_BUS();
	_delay_us(OWI_DELAY_I);
	result = ~OWI_PIN & OWI;
	_delay_us(OWI_DELAY_J);
	
	RESTORE_INTERRUPT();
	
	return result ? true : false;
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Writes '1' on the bus.
 * @param    None
 * @retval   None
 */
static void OWIMaster_WriteBit1(void)
{
	SAVE_INTERRUPT();
	
	OWI_PULL_BUS_LOW();
	_delay_us(OWI_DELAY_A);
	OWI_RELEASE_BUS();
	_delay_us(OWI_DELAY_B);
	
	RESTORE_INTERRUPT();
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Writes 'o' on the bus.
 * @param    None
 * @retval   None
 */
static void OWIMaster_WriteBit0(void)
{
	SAVE_INTERRUPT();
	
	OWI_PULL_BUS_LOW();
	_delay_us(OWI_DELAY_C);
	OWI_RELEASE_BUS();
	_delay_us(OWI_DELAY_D);
	
	RESTORE_INTERRUPT();
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Reads bit on the bus
 * @param    None
 * @retval   Read bit (in OWI bit mask format)
 */
static uint8_t OWIMaster_ReadBit(void)
{
	uint8_t result;
	
	SAVE_INTERRUPT();
	
	OWI_PULL_BUS_LOW();
	_delay_us(OWI_DELAY_A);
	OWI_RELEASE_BUS();
	_delay_us(OWI_DELAY_E);
	result = OWI_PIN & OWI;
	_delay_us(OWI_DELAY_F);
	
	RESTORE_INTERRUPT();
	
	return result;
}

/*----------------------------------------------------------------------------*/
void OWIMaster_SendByte(uint8_t byte)
{
	uint8_t idx;
	
	for (idx = 0 ; idx < 8; idx++)
	{
		if (byte & 0x01)
		{
			OWIMaster_WriteBit1();
		}
		else
		{
			OWIMaster_WriteBit0();
		}
		
		byte >>= 1;
	}
}

/*----------------------------------------------------------------------------*/
uint8_t OWIMaster_ReadByte(void)
{
	uint8_t result = 0;
	uint8_t idx;
	
	// Bit loop
	for (idx = 0; idx < 8; idx++)
	{
		result >>= 1;
		
		if (OWIMaster_ReadBit())
		{
			result |= 0x80;
		}
	}
	
	return result;
}

/*----------------------------------------------------------------------------*/
void OWIMaster_ReadBytes(uint8_t *data, uint8_t length)
{
	uint8_t idx;
	
	for (idx = 0; idx < length; idx++)
	{
		*data++ = OWIMaster_ReadByte();
	}
}

/*----------------------------------------------------------------------------*/
void OWIMaster_SendBytes(uint8_t *data, uint8_t length)
{
	uint8_t idx;
	
	for (idx = 0; idx < length; idx++)
	{
		OWIMaster_SendByte(*data++);
	}
}

/*----------------------------------------------------------------------------*/
void OWIMaster_SkipRom(void)
{
	OWIMaster_SendByte(OWIC_SKIP_ROM);
}

/*----------------------------------------------------------------------------*/
bool OWIMaster_ReadRomCode(OWIROMCode_t *romCode)
{
	OWIMaster_Init();
	OWIMaster_SendByte(OWIC_READ_ROM);
	OWIMaster_ReadBytes(romCode->ID, sizeof(OWIROMCode_t));
	
	return OWI_CheckROMCode(romCode->ID);
}

/*----------------------------------------------------------------------------*/
void OWIMaster_SendRomCode(OWIROMCode_t *romCode)
{
	OWIMaster_Init();
	OWIMaster_SendByte(OWIC_MATCH_ROM);
	OWIMaster_SendBytes(romCode->ID, sizeof(OWIROMCode_t));
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Sends SEARCH ROM command
 * @param    *romId : wskaźnik do tablicy przechowującej aktualnie odczytany kod buffer for actual code
 * @param    lastDeviation : last bit position selected by searching algorithm
 *           (0 at the first iteration)
 * @retval : Last bit position for difference
 */
static uint8_t OWIMaster_SearchRom(uint8_t *romId, uint8_t lastDeviation)
{
	uint8_t currentBit = 1;
    uint8_t newDeviation = 0;
    uint8_t bitMask = 0x01;
    uint8_t bitA;
    uint8_t bitB;
	
    OWIMaster_SendByte(OWIC_ROM_SEARCH);	
    
    // All 64 bits analysis
    while (currentBit <= 64)
    {
        // Double bit read
        bitA = OWIMaster_ReadBit();
        bitB = OWIMaster_ReadBit();

        if (bitA && bitB)
        {
            // Error
            return 0;
        }
        else if (bitA ^ bitB)
        {
			// All the devices have the same bits at this position.
            if (bitA)
            {
                (*romId) |= bitMask;
            }
            else
            {
                (*romId) &= ~bitMask;
            }
        }
        else						// Both bits 0
        {            
            if (currentBit == lastDeviation)
            {
				// If it's last selected position, '1' this time will be chosen.
				(*romId) |= bitMask;
            }
            else if (currentBit > lastDeviation)
            {
				// For the remaining ID's '0' selected when deviation is detected.
				(*romId) &= ~bitMask;
                newDeviation = currentBit;
            }            
            else if ( !(*romId & bitMask)) 
            {
				// Looking for new deviation
				newDeviation = currentBit;
            }
        }               
        
        // Sends selected bit
        if ((*romId) & bitMask)
        {
            OWIMaster_WriteBit1();
        }
        else
        {
            OWIMaster_WriteBit0();
        }
          
        currentBit++;						// Next bit preparation

		// Establishing pointer for bitMask and bitPattern
        bitMask <<= 1;
        if (!bitMask)
        {
            bitMask = 0x01;
            romId++;
        }
    }
	
    return newDeviation;
}

/*----------------------------------------------------------------------------*/
bool OWIMaster_IsFamilyCodeExist(EOWIFamily_t *codes, EOWIFamily_t familyCode)
{
	bool result = false;
	
	while (*codes != OWIF_EOL)
	{
		if (*codes == familyCode ||
		    *codes == OWIF_ALL)
		{
			result = true;
			break;
		}
		
		codes++;
	}
	
	return result;
}

/*----------------------------------------------------------------------------*/
uint8_t OWIMaster_SearchBuses(OWIDevice_t *devices, 
                                     uint8_t length,
							         EOWIFamily_t *familyCodes)
{
	uint8_t i, j;
    uint8_t lastDeviation;
    uint8_t numDevices = 0;
	uint8_t tempId[9];
    
    for (i = 0; i < length; i++)
    {
        for (j = 0; j < 8; j++)
        {
            devices[i].ROMCode.ID[j] = 0x00;
        }
    }
    
    lastDeviation = 0;
    if (OWIMaster_Init())
    {
		do  
        {
            OWIMaster_Init();
            lastDeviation = OWIMaster_SearchRom(tempId, lastDeviation);
												
			if (OWIMaster_IsFamilyCodeExist(familyCodes, 
			                                tempId[0]))
			{
				if (OWI_CheckROMCode(tempId))
				{					
					memcpy(devices[numDevices].ROMCode.ID, tempId, 8);
					devices[numDevices].IsExist = true;
					numDevices++;
				}				
			}												
        }  while (numDevices < length && lastDeviation);            
    }
	
	return numDevices;	
}

/******************* (C) COPYRIGHT 2011 HENIUS *************** END OF FILE ****/
