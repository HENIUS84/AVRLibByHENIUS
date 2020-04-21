/**
 *******************************************************************************
 * @file     OWIMaster.h
 * @author   HENIUS (Paweł Witak)
 * @version  1.1.1
 * @date     11-06-2011
 * @brief    1-Wire driver in Master mode (header file)
 *******************************************************************************
 *
 * <h2><center>COPYRIGHT 2011 HENIUS</center></h2>
 */

#ifndef OWI_MASTER_H_
#define OWI_MASTER_H_

/* Include section -----------------------------------------------------------*/

// --->System files

#include <stdbool.h>
#include <stdint.h>

/* Macros, constants and definitions section ---------------------------------*/

// Bus configuration
// Ports
#define OWI_PORT			(PORTB)	/*!< Output register of 1-Wire port */
#define OWI_DDR				(DDRB)	/*!< Direction register of 1-Wire port */
#define OWI_PIN				(PINB)	/*!< Output register of 1-Wire */
#define OWI					(_BV(2))/*!< Mask of 1-Wire pin */
//#define OWI_INT_PULLUP			/*!< Aktywacja wewnętrznego pullup'a */
/*! Bus releasing - pin in input mode */
#ifdef OWI_INT_PULLUP
#define OWI_RELEASE_BUS()	OWI_DDR &= ~OWI;\
                            OWI_PORT |= OWI
#else
#define OWI_RELEASE_BUS()	OWI_DDR &= ~OWI;\
                            OWI_PORT &= ~OWI
#endif 
/*! Bus pullup to GND */
#define OWI_PULL_BUS_LOW()	OWI_DDR |= OWI;\
						    OWI_PORT &= ~OWI
							
// Macros
/*! Saves interrupt states and enable it */
#define SAVE_INTERRUPT()	IntState = SREG;\
                            cli()
/*! Interrupt restoration */
#define RESTORE_INTERRUPT()	SREG = IntState

// --->Types

/**
 * @brief Family codes in 1-Wire
 */
typedef enum
{
	// Thermometers
	/*! Digital thermometer with programmable resolution */
	OWIF_DS18B20 = 0x28,	
	/*! Digital thermometer with high precisions */		
	OWIF_DS18S20 = 0x10,
	/*! Digital thermometer with programmable resolution */
	OWIF_DS1822  = 0x22,
			
	OWIF_ALL = 0xFE,						/*!< Any device */
	OWIF_EOL								/*!< End of list */
}EOWIFamily_t;

/**
 * @brief ID structure of 1-Wire device
 */
typedef union
{
	struct
	{		
		EOWIFamily_t Family : 8;			/*!< Family code */
		uint8_t SerialNumber[6];			/*!< Serial number */
		uint8_t	Crc;						/*!< Checksum */
	};	
	
	uint8_t ID[8];							/*!< Table of ID bytes */	
}OWIROMCode_t;

/**
 * @brief 1-Wire device
 */
typedef struct
{
	OWIROMCode_t ROMCode;					/*!< Device ID */
	/*! Transmission status (true - OK) */
	bool IsExist;
}OWIDevice_t;

// --->Constants

// Time constants (in us)
#define OWI_DELAY_A			(6)		/*< Time constant type A */   
#define OWI_DELAY_B			(64)	/*< Time constant type B */ 
#define OWI_DELAY_C			(60)	/*< Time constant type C */ 
#define OWI_DELAY_D			(10)	/*< Time constant type D */ 
#define OWI_DELAY_E			(9)		/*< Time constant type E */ 
#define OWI_DELAY_F			(55)	/*< Time constant type F */ 
#define OWI_DELAY_G			(0)		/*< Time constant type G */
#define OWI_DELAY_H			(480)	/*< Time constant type H */ 
#define OWI_DELAY_I			(70)	/*< Time constant type I */ 
#define OWI_DELAY_J			(410)	/*< Time constant type J */ 

// 1-Wire ROM commands
#define OWIC_READ_ROM   	(0x33)	/*!< Command READ ROM */
#define OWIC_SKIP_ROM		(0xCC)	/*!< Command SKIP ROM */
#define OWIC_MATCH_ROM		(0x55)	/*!< Command MATCH ROM */
#define OWIC_ROM_SEARCH		(0xF0)	/*!< Command SEARCH ROM */

/* Declaration section -------------------------------------------------------*/

// --->Functions

/*----------------------------------------------------------------------------*/
/**
 * @brief    Initializes 1-Wire bus (RESET i PRESENCE signal)
 * @param    None
 * @retval   Flag of SLAVE existence (0 - no Slave device)
 */
bool OWIMaster_Init(void);

/*----------------------------------------------------------------------------*/
/**
 * @brief    Reads ROM code (only if one SLAVE detected)
 * @param    *romCode : code buffer 
 * @retval   True if checksum is Ok
 */
bool OWIMaster_ReadRomCode(OWIROMCode_t *romCode);

/*----------------------------------------------------------------------------*/
/**
 * @brief    Sends ROM code (device addressing)
 * @param    *romCode : ROM code buffer
 * @retval   None
 */
void OWIMaster_SendRomCode(OWIROMCode_t *romCode);

/*----------------------------------------------------------------------------*/
/**
 * @brief    Reads byte from 1-Wire
 * @param    None
 * @retval : Read byte
 */
uint8_t OWIMaster_ReadByte(void);

/*----------------------------------------------------------------------------*/
/**
 * @brief    Reads bytes over 1-Wire bus
 * @param    *data : data buffer
 * @param    length : bytes amount to read
 * @retval   None
 */
void OWIMaster_ReadBytes(uint8_t *data, uint8_t length);

/*----------------------------------------------------------------------------*/
/**
 * @brief    Sends bytes over po 1-Wire bus
 * @param    *data : data to save
 * @param    length : bytes amount to save
 * @retval   None
 */
void OWIMaster_SendBytes(uint8_t *data, uint8_t length);

/*----------------------------------------------------------------------------*/
/**
 * @brief    Sends SKIP ROM command over 1-Wire bus
 * @param    None
 * @retval   None
 */
void OWIMaster_SkipRom(void);

/*----------------------------------------------------------------------------*/
/**
 * @brief    Sends byte over 1-Wire bus
 * @param    byte : byte to send
 * @retval   None
 */
void OWIMaster_SendByte(uint8_t byte);

/*----------------------------------------------------------------------------*/
/**
 * @brief    Searches for SLAVE devices on 1-Wire bus
 * @param    *devices : buffer of found device ID's
 * @param    length : max number of devices to be found
 * @param    *familyCodes : list of codes to be found
 * @retval : Number of found devices (0 - no device or bus error)
 */
uint8_t OWIMaster_SearchBuses(OWIDevice_t *devices, 
                              uint8_t length,
							  EOWIFamily_t *familyCodes);

/*----------------------------------------------------------------------------*/
/**
 * @brief    Checks if current family code is known code
 * @param    familyCode : searched code
 * @param    *codes : list of known codes
 * @retval : Result (true - known device)
 */
bool OWIMaster_IsFamilyCodeExist(EOWIFamily_t *codes, EOWIFamily_t familyCode);

#endif 								/* OWI_MASTER_H_ */

/******************* (C) COPYRIGHT 2011 HENIUS *************** END OF FILE ****/
