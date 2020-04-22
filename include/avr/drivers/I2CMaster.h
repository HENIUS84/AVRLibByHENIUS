/**
 *******************************************************************************
 * @file     I2CMaster.h                                                      
 * @author   HENIUS (Paweł Witak)                                              
 * @version  1.1.3
 * @date     03/04/2014                                                        
 * @brief    I2C Master driver (header file)                                  
 *******************************************************************************
 *
 * <h2><center>COPYRIGHT 2014 HENIUS</center></h2>
 */

#ifndef  I2C_MASTER_H
#define  I2C_MASTER_H

/* Include section -----------------------------------------------------------*/

// --->System files

#include <stdbool.h>
#include <stdint.h>
#include <math.h>

// --->User files

/* Macros, constants and definitions section ---------------------------------*/

// --->Constants

#define I2C_MAX_OF_TWPS		(2)		/*!< Max value of register TWPS */
#define I2C_SCK_MAX_ERROR	(10000)	/*!< Max bus frequency error (in Hz) */
/*! Waiting time for transceiver releasing */
#define I2C_TRANSCEIVER_BUSY_TIMEOUT	(10000)

// Bus settings

#define I2C_BUFFER_SIZE    	(100)	/*!< Size of read and write buffer*/
/*! Position of Address field in SLA+R/W byte */	
#define I2C_ADDR_BITS		(1)		
/*! Position of R/!W bit in address byte */
#define I2C_READ_BIT  		(_BV(I2C_ADDR_BITS - 1))
//#define I2C_DEBUG_ENABLED			/*!< DEBUG mode activation for I2C driver */


// --->Macros

/*! Calculation of transmission clock frequency */
#define I2C_GET_FREQUENCY(CPU_Freq, twbr_reg, twps_reg) \
	round((double)CPU_Freq / (16 + twbr_reg * (2 << (2 * twps_reg))))
/*! Calculation of TWBR register value */
#define I2C_GET_TWBR(CPU_Freq, I2C_Freq, twps_reg) \
	round(((double)CPU_Freq - 16 * I2C_Freq)/ ((2 << (2 * twps_reg)) * I2C_Freq))

/*! I2C bus status */
#define I2C_STATUS			(TWSR & 0xF8)

// --->Types

/**
 * @brief I2C bus states
 */
typedef enum
{
	// Main codes of TWI Master statuses
	
	I2C_START         = 0x08,  		/*!< START bit sent */
	/*! Double START bit transmitted */
	I2C_REP_START     = 0x10,   	
	I2C_ARB_LOST      = 0x38,   	/*!< Arbitration  issue */

	// Codes of statuses of TWI Master transmitter
	I2C_MTX_ADR_ACK   = 0x18,  		/*!< SLA+W sent and ACK received */
	I2C_MTX_ADR_NACK  = 0x20,  		/*!< SLA+W sent and i NACK received */
	/*! Data byte sent and ACK received */
	I2C_MTX_DATA_ACK  = 0x28,  	
	/*! Data byte sent and NACK received */	
	I2C_MTX_DATA_NACK = 0x30,       

	// Codes of statuses of TWI Master receiver
	
	I2C_MRX_ADR_ACK   = 0x40,  		/*!< SLA+R sent and ACK received */
	I2C_MRX_ADR_NACK  = 0x48,  		/*!< SLA+R sent and NACK received */
	/*!Data byte received and ACK sent */
	I2C_MRX_DATA_ACK  = 0x50,  		
	/*! Data byte received and NACK sent */
	I2C_MRX_DATA_NACK = 0x58,  		
	
	// Remaining codes of TWI status
	I2C_NO_STATE      = 0xF8,  		/*!< No relevant state */
	/*! Bus error due to improper START and STOP bit conditions */
	I2C_BUS_ERROR     = 0x00,  		
}EI2CState_t;

/**
 * @brief Connection state of I2C driver
 */
typedef enum
{
	I2CMS_OK,						/*!< Status OK */
	I2CMS_TRANSCEIVER_NOT_READY		/*!< Receiver not ready */
}EI2CMasterStatus_t;

/**
 * @brief I2C clock frequency
 */
typedef enum
{
	I2CC_400K = 400000,				/*!< 400kHz */
	I2CC_200K = 200000,				/*!< 200kHz */
	I2CC_100K = 100000,				/*!< 100kHz */
	I2CC_50K  = 50000				/*!< 50 kHz */	
}EI2Cclock_t;

/**
 * @brief Bus status register
 */
typedef union
{
   uint8_t All;

   struct
	{
		uint8_t IsLastSendingOk   : 1;		/*!< Last transmission state */
		uint8_t IsLastReceivingOk : 1;		/*!< Last receiving state */
		uint8_t UnusedBits  : 6;			/*!< Unused bits */
	 };
}I2CStatusReg_t;

/**
 * @brief I2C bus configuration
 */
typedef struct
{
	EI2Cclock_t ClockRate;					/*!< Clock frequency in Hz */		
	uint32_t CpuFrequency;					/*!< CPU frequency in MHz */
}I2CMaster_t;

/* Declaration section -------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/**
 * @brief    Master module initialization
 * @param    *i2cConfig : pointer to the configuration
 * @retval   None
 */
void I2CMaster_Init(I2CMaster_t* i2cConfig);

/*----------------------------------------------------------------------------*/
/**
 * @brief    Deinitializes I2C module
 * @param    None
 * @retval   None
 */
void I2CMaster_Deinit(void);

/*----------------------------------------------------------------------------*/
/**
 * @brief    Checks if transceiver is busy
 * @param    None
 * @retval   None
 */
bool I2CMaster_IsTransceiverBusy();

/*----------------------------------------------------------------------------*/
/**
 * @brief    Sends data to the Slave (first byte is address).
 * @param    message: message buffer pointer
 * @param    messageSize: bytes number to be sent
 * @retval   Operation status (true - success)
 */
bool I2CMaster_SendData(uint8_t *message, uint8_t messageSize);

/*----------------------------------------------------------------------------*/
/**
 * @brief    Reads the buffer data from interface
 * @param    message: buffer for receiving data
 * @param    messageSize: bytes number to be received
 * @retval   Operation status (true - success)
 */
bool I2CMaster_ReadData(uint8_t *message, uint8_t messageSize);

/*----------------------------------------------------------------------------*/
/**
 * @brief    Gets the I2C transmission state
 * @param    None
 * @retval   Transmission state
 */
I2CStatusReg_t I2CMaster_GetStatus();

#endif										/* I2C_MASTER_H */

/******************* (C) COPYRIGHT 2014 HENIUS *************** END OF FILE ****/
