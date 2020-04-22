/**
 *******************************************************************************
 * @file     OWIThermometermeter.c
 * @author   HENIUS (Paweł Witak)
 * @version  1.1.2
 * @date     10/04/2013
 * @brief    Driver of 1-Wire thermometer
 *******************************************************************************
 *
 * <h2><center>COPYRIGHT 2013 HENIUS</center></h2>
 */

/* Include section -----------------------------------------------------------*/

// --->System files

#include <stdint.h>
#include <stdbool.h>
#include <util/delay.h>

// --->User files

#include "OWIMaster.h"
#include "OWIThermometer.h"
#include "OWICrc.h"

/* Variable, macros and constants section ------------------------------------*/

// --->Variables

static OWIThermoCtrl_t *Thermometers;		/*!< Table of 1-Wire devices */
/*!< Waiting timer for temperature ready */
static uint16_t ThermDelayTimer;	
/*! Waiting time for temperature ready */	
static uint16_t ThermoDelay;			
/*!< Waiting timer for next temperature measurement */
static uint16_t ThermoIntervalTimer;
/*! Waiting time for next measurement */
static uint16_t ThermoInterval;
/*! Codes of thermometers */
EOWIFamily_t ThermoDeviceFC[] =		
{
	OWIF_DS18B20,
	OWIF_DS18S20,
	OWIF_DS1822,
	OWIF_EOL
};

/* Function section ----------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
void OWIThermo_Init(OWIThermoCtrl_t *thermometers)
{
	OWIThermoConfig_t configuration;		
	
	Thermometers = thermometers;
		
	// Gets device addresses.
	if (Thermometers->MaxAmountOfDevices == 1)
	{
		// Only one sensor on the bus.
		if (OWIMaster_ReadRomCode(&Thermometers->Devices[0].ROMCode) &&
		    OWIMaster_IsFamilyCodeExist(
				ThermoDeviceFC, 
			    Thermometers->Devices[0].ROMCode.Family))
		{ 			
			Thermometers->AmountOfFoundDevices = 1;
		}
	}
	else if (Thermometers->MaxAmountOfDevices > 1)
	{
		Thermometers->AmountOfFoundDevices = 
			OWIMaster_SearchBuses(Thermometers->Devices, 
			                      Thermometers->MaxAmountOfDevices, 
								  ThermoDeviceFC);								  
	}
	
	// Measurement resolution setting
	OWIMaster_SkipRom();
	configuration.ConfigReg.Resolution = Thermometers->Resolution;
	ThermoDelay = OWI_TEMP_CONV_TIME / (0x08 >> Thermometers->Resolution);
	if (Thermometers->RepetitionTime <= ThermoDelay)
	{
		ThermoInterval = Thermometers->TaskTime;
	}
	else
	{
		ThermoInterval = Thermometers->RepetitionTime - ThermoDelay;
	}
	
	// Timer settings
	ThermoDelay /= Thermometers->TaskTime;
	ThermoInterval /= Thermometers->TaskTime;
	ThermDelayTimer = ThermoDelay;
	ThermoIntervalTimer = ThermoInterval;
	
	OWIThermo_WriteConfig(&configuration, 0);	
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Convert the temperature
 * @param    *thermMem : sensor memory pointer
 * @param    family : sensor type
 * @retval   Temperature (with sign)
 */
static int16_t OWITherm_ConvertTemp(OWIThermoMem_t *thermMem,
                                    EOWIFamily_t family)
{
	uint16_t bitMask = 0;
	
	switch (family)
	{
		// --->Programable sensors
		case OWIF_DS18B20:
		case OWIF_DS1822:
			bitMask = 0x07FF;
		
			break;
		
		// --->Sensors with high resolutions
		case OWIF_DS18S20:
			bitMask = 0x00FF;
		
			break;
			
		default:
			break;
	}
	
	return 
		(thermMem->Temperature & bitMask) * 
		((thermMem->Temperature & 0x8000) ? -1 : 1);
}

/*----------------------------------------------------------------------------*/
bool OWIThermo_ReadMemory(OWIThermoMem_t *memory, OWIROMCode_t *romId)
{
	bool result = false;
	
	// Bus initialization
	if (OWIMaster_Init())
	{
		// Sensor addressing
		if (!romId)
		{
			OWIMaster_SkipRom();
		}
		else
		{
			OWIMaster_SendRomCode(romId);
		}
		
		// Configuration write (depends on sensor type)
		OWIMaster_SendByte(OWITC_READ_MEM);
		OWIMaster_ReadBytes(memory->All, sizeof(OWIThermoMem_t));
		
		result = !OWI_CalculateCRC(memory->All, sizeof(OWIThermoMem_t)) ?
			true : false;
	}
	
	return result;
}

/*----------------------------------------------------------------------------*/
void OWIThermo_WriteConfig(OWIThermoConfig_t *config, OWIROMCode_t *romId)
{
	// Bus initialization
	if (OWIMaster_Init())
	{
		// Sensor addressing
		if (!romId)
		{
			OWIMaster_SkipRom();
		}
		else
		{
			OWIMaster_SendRomCode(romId);
		}
		
		// Configuration write (depends on sensor type)
		OWIMaster_SendByte(OWITC_WRITE_MEM);
		OWIMaster_SendBytes(config->All, romId->Family == OWIF_DS18S20 ? 
			sizeof(OWIThermoConfig_t) - 1 : sizeof(OWIThermoConfig_t));
	}
}

/*----------------------------------------------------------------------------*/
int16_t OWIThermo_ReadTemperature(OWIROMCode_t *romID)
{
	OWIMaster_Init();
	
	OWIThermoMem_t thermMem;
	int16_t temperature = 0;
	
	// SKIP ROM if no ID specified
	if (!romID)
	{
		OWIMaster_SkipRom();		
	}
	else
	{
		OWIMaster_SendRomCode(romID);
	}
	
	OWIMaster_SendByte(OWITC_START_CONV);
	Thermometers->Delay_ms(ThermoDelay);
	
	if (OWIThermo_ReadMemory(&thermMem, romID))
	{
		temperature = OWITherm_ConvertTemp(&thermMem, romID->Family);
	}
	
	return temperature;
}

/*----------------------------------------------------------------------------*/
void OWIThermo_Handler(void)
{
	static EOWIThermoState_t currStates = OWITS_WAIT_FOR_NEXT;
	static bool readScratchpad = false;	
	static uint8_t devNumber;
	static uint8_t byteNumber;	
	static OWIThermoMem_t thermMem;	
	uint8_t amountOfDevice = 
		Thermometers->AmountOfFoundDevices >= Thermometers->MaxAmountOfDevices ?
		Thermometers->MaxAmountOfDevices : Thermometers->AmountOfFoundDevices;
	
	switch (currStates)
	{
		// Measurement initialization
		case OWITS_INIT:
			currStates = OWITS_SKIP_ROM;						
			OWIMaster_Init();
									
			break;
			
		// Skip ROM code
		case OWITS_SKIP_ROM:
			if (!readScratchpad)
			{
				currStates = OWITS_START_CONV;
				OWIMaster_SkipRom();
			}
			else
			{
				if (amountOfDevice == 1)
				{
					currStates = OWITS_READ_MEM_REQUEST;
					OWIMaster_SkipRom();	
				}
				else if (amountOfDevice > 1)
				{
					currStates = OWITS_MATCH_ROM;
				}
			}					
		
			break;
			
		// Conversion request
		case OWITS_START_CONV:
			currStates = OWITS_WAIT;
			OWIMaster_SendByte(OWITC_START_CONV);	
					
			break;
			
		// Waiting for conversion
		case OWITS_WAIT:
			if (!(--ThermDelayTimer))
			{
				currStates = OWITS_INIT;				
				ThermDelayTimer = ThermoDelay;;
				readScratchpad = true;
			}
		
			break;
		
		// Device address sending
		case OWITS_MATCH_ROM:
			OWIMaster_SendRomCode(&Thermometers->Devices[devNumber].ROMCode);
			Thermometers->Devices[devNumber].IsExist = true;
			currStates = OWITS_READ_MEM_REQUEST;
		
			break;
		
		// Memory read request
		case OWITS_READ_MEM_REQUEST:
			currStates = OWITS_READ_MEM;
			OWIMaster_SendByte(OWITC_READ_MEM);
			byteNumber = 0;
			readScratchpad = false;		
		
			break;
		
		// Scratchpad read request
		case OWITS_READ_MEM:
			currStates = OWITS_READ_MEM;
			thermMem.All[byteNumber] = OWIMaster_ReadByte(); 			
			byteNumber++;
			
			if (byteNumber == sizeof(OWIThermoMem_t))
			{
				ThermDelayTimer = ThermoDelay;
				
				// CRC calculation
				Thermometers->Devices[devNumber].IsExist = 
					!OWI_CalculateCRC(thermMem.All, sizeof(OWIThermoMem_t)) ? 
						true : false;
				
				if (Thermometers->Devices[devNumber].IsExist)
				{		
					Thermometers->Temperatures[devNumber] = 
						OWITherm_ConvertTemp( 
							&thermMem, 
						    Thermometers->Devices[devNumber].ROMCode.Family);								
				}
				
				if (devNumber == (amountOfDevice - 1))
				{
					currStates = OWITS_WAIT_FOR_NEXT;
					devNumber = 0;
				}
				else
				{
					currStates = OWITS_INIT;
					readScratchpad = true;
					devNumber++;
				}	
			}
		
			break;	
			
		// Waiting for next measurement
		case OWITS_WAIT_FOR_NEXT:
			if (!(--ThermoIntervalTimer))
			{	
				currStates = OWITS_INIT;
				ThermoIntervalTimer = ThermoInterval;				
			}
		
			break;
		
		// Unknown state	
		default:
			currStates = OWITS_INIT;
		
			break;
	}
}

/******************* (C) COPYRIGHT 2013 HENIUS *************** END OF FILE ****/
