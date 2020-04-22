/**
 *******************************************************************************
 * @file     OWIThermometermeter.c
 * @author   HENIUS (Paweł Witak)
 * @version  1.01.002
 * @date     10/04/2013
 * @brief    Driver of 1-Wire thermometer (header file)
 *******************************************************************************
 *
 * <h2><center>COPYRIGHT 2013 HENIUS</center></h2>
 */

#ifndef OWI_THERMOMETER_H_
#define OWI_THERMOMETER_H_

/* Include section -----------------------------------------------------------*/

// --->System files

#include <stdbool.h>

// --->User files

#include "OWIMaster.h"

/* Macros, constants and definitions section ---------------------------------*/

// --->Constants

// ROM commands for thermometers
#define OWITC_START_CONV   	(0x44)	/*!< Conversion start */
#define OWITC_READ_MEM		(0xBE)	/*!< Read scratchpad memory (9 bytes) */
#define OWITC_WRITE_MEM		(0x4E)	/*!< Write sensor memory (3 bytes) */
#define OWITC_COPY_MEM		(0x48)	/*!< Copy configuration to the EEPROM */
#define OWITC_RECALL_EEPROM	(0xB8)	/*!< Copy configuration from EEPROM'u */
#define OWITC_READ_POWER	(0xB4)	/*!< Read power state */

#define OWI_TEMP_CONV_TIME	(750)	/*!< Conversion time in ms */

// --->Types

/**
 * @brief Temperature measurement resolution
 */
typedef enum
{
	OWITR_9BIT,						/*!< 9 bits resolution */
	OWITR_10BIT,					/*!< 10 bits resolution */
	OWITR_11BIT,					/*!< 11 bits resolution */
	OWITR_12BIT						/*!< 12 bits resolution */
}OWITempRes_t;

/**
 * @brief Initialization structure for thermometer
 */
typedef struct
{	
	uint8_t MaxAmountOfDevices;		/*!< Max number of devices to be detected */
	uint8_t AmountOfFoundDevices;	/*!< Number of found devices */
	uint8_t TaskTime;				/*!< Task calculation duration (in ms) */	
	void(*Delay_ms)(uint16_t delay);/*!< Pointer to delay function */	
	OWITempRes_t Resolution;		/*!< Measurement resolution */
	uint16_t RepetitionTime;		/*!< Sensor repetition time (in ms) */
	OWIDevice_t *Devices;			/*!< Device list */
	int16_t *Temperatures;			/*!< Temperatures list */
}OWIThermoCtrl_t;

/**
 * @brief Configuration structure for sensor
 */
typedef union
{
	struct
	{
		uint8_t THReg;				/*!< TH register or User Byte 1 */
		uint8_t TLReg;				/*!< TL register or User Byte 2 */	
		struct 
		{
			uint8_t UnusedBits : 5;	/*!< Unused bits */
			uint8_t Resolution : 2;	/*!< Resolution settings */
			uint8_t UnusedBit  : 1;	/*!< Unused bits */
		}ConfigReg;					/*!< Configuration register */
	};	
	
	uint8_t All[3];					/*!< Configuration table */
}OWIThermoConfig_t;

/**
 * @brief Structure of sensor memory description ((*) depends on current sensor)
 */
typedef union
{
	struct
	{
		uint16_t Temperature;		/*!< Sensor temperature */
		/*! Configuration register with EEPROM write functionality (*) */
		OWIThermoConfig_t EEPROMSpace;
		uint8_t Reserved;			/*!< Reserved */
		uint8_t CountRemain;		/*!< Counter reaming value */
		uint8_t CountPerDegree;		/*!< Pulse count for one degree */
		uint8_t CRC;				/*!< Checksum */
	};
		
	uint8_t All[9];					/*!< Sensor memory table */
}OWIThermoMem_t;

/**
 * @brief States of temperature measurement
 */
typedef enum
{
	OWITS_INIT,						/*!< Initialization */
	OWITS_SKIP_ROM,					/*!< ROM code serving skip */
	OWITS_START_CONV,				/*!< Temperature measurement request */
	OWITS_WAIT,						/*!< Waiting for conversion */
	OWITS_WAIT_FOR_NEXT, 			/*!< Waiting for next measurement */
	OWITS_READ_MEM_REQUEST,			/*!< Scratchpad memory reading request */
	OWITS_READ_MEM,     			/*!< Reading of scratchpad memory */
	OWITS_MATCH_ROM					/*!< Device code sending */
}EOWIThermoState_t;

/* Declaration section -------------------------------------------------------*/

// --->Variables

// List of family codes for thermometers
extern EOWIFamily_t ThermoDeviceFC[];

// --->Functions

/*----------------------------------------------------------------------------*/
/**
 * @brief    Initializes 1-Wire thermometer
 * @param    *thermometers : table of thermometrs
 * @retval   None
 */
void OWIThermo_Init(OWIThermoCtrl_t *thermometers);

/*----------------------------------------------------------------------------*/
/**
 * @brief    Temperature measurement handler
 * @param    None
 * @retval   None
 */
void OWIThermo_Handler(void);

/*----------------------------------------------------------------------------*/
/**
 * @brief    Writes the configuration.
 * @param    *config : configuration pointer
 * @oaram	 *romId : pointer to the ID (0 - to Skip Rom)
 * @retval   None
 */
void OWIThermo_WriteConfig(OWIThermoConfig_t *config, OWIROMCode_t *romId);

/*----------------------------------------------------------------------------*/
/**
 * @brief    Reads sensor memory
 * @param    *memory : pointer to the memory structure
 * @oaram	 *romId : pointer to the sensor ID (0 - to Skip Rom)
 * @retval   Checksum status (true - no issues)
 */
bool OWIThermo_ReadMemory(OWIThermoMem_t *memory, OWIROMCode_t *romId);

/*----------------------------------------------------------------------------*/
/**
 * @brief    Reads temperature for current sensor
 * @param    *romID : sensor ID
 * @retval   Sensor temperature
 */
int16_t OWIThermo_ReadTemperature(OWIROMCode_t *romID);

#endif 								/* OWI_THERMOMETER_H_ */

/******************* (C) COPYRIGHT 2013 HENIUS *************** KONIEC PLIKU ***/
