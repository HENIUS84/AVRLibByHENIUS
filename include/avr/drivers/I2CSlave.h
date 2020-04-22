/**
 *******************************************************************************
 * @file     I2CSlave.h                                                       
 * @author   HENIUS (Paweł Witak)                                              
 * @version  1.01.001                                                          
 * @date     04/03/2011                                                        
 * @brief    I2CSalve driver (header file)                                    
 *******************************************************************************
 *
 * <h2><center>COPYRIGHT 2011 HENIUS</center></h2>
 */

#ifndef  I2C_SLAVE_H
#define  I2C_SLAVE_H

/* Include section -----------------------------------------------------------*/

// --->System files

#include <stdbool.h>

/* Macros, constants and definitions section ---------------------------------*/

//--->Constants

// Bus settings
#define I2C_BUFFER_SIZE    	(100)			/*!< TX buffer size */
/*! Position of address field in SLA+R/W byte */
#define I2C_ADDR_BITS		(1)

// --->Macros

/*! Reset timeout timer */
#define I2C_TIMEOUT_RESET()	(I2Ctimeout = I2C_TIMEOUT)

/* ! I2C bus status */
#define I2C_STATUS			(TWSR & 0xF8)

// --->Types

/**
 * @brief I2C interface state
 */
typedef enum
{
	// Codes of I2C slave transmitter
	I2C_STX_ADR_ACK            = 0xA8,		/*!< Received own SLA+R, ACK sent */
	/*! Arbitration lost in SLA+R/W (Master), received SLA+R, ACK sent */
	I2C_STX_ADR_ACK_M_ARB_LOST = 0xB0,
	/*! Byte from TWDR sent, ACK received */
	I2C_STX_DATA_ACK           = 0xB8,
	/*! Byte from TWDR sent, NACK received */
	I2C_STX_DATA_NACK          = 0xC0,
	/*! Last byte from TWDR sent (TWEA = 0), ACK received */
	I2C_STX_DATA_ACK_LAST_BYTE = 0xC8,

	// Status codes of I2C Slave receiver
	I2C_SRX_ADR_ACK            = 0x60,		/*!< Own SLA+W received, ACK sent */
	/*! Arbitration lost in SLA+R/W (Master), SLA+W received, ACK sent */
	I2C_SRX_ADR_ACK_M_ARB_LOST = 0x68,
	/*! Main call address received, ACK sent */  
	I2C_SRX_GEN_ACK            = 0x70,
	/*!< Arbitration lost in SLA+R/W (Master), main call address received, ACK sent */
	I2C_SRX_GEN_ACK_M_ARB_LOST = 0x78,
	/*! Previously addressed SLA+W data received, ACK sent */
	I2C_SRX_ADR_DATA_ACK       = 0x80,
	/*! Previously addressed SLA+W data received, NACK sent */
	I2C_SRX_ADR_DATA_NACK      = 0x88,
	/*! Previously addressed main call data received, ACK sent */
	I2C_SRX_GEN_DATA_ACK       = 0x90,
	/*! Previously addressed main call data received, NACK sent */
	I2C_SRX_GEN_DATA_NACK      = 0x98,
	/*! STOP condition or repeated START received during still addressed SLAVE */  
	I2C_SRX_STOP_RESTART       = 0xA0,  

	//Other I2C statuses
	I2C_NO_STATE               = 0xF8,		/*!< No state */
	/*! Bus error due to improper START or condition STOP */
	I2C_BUS_ERROR              = 0x00,  

}EI2CState_t;

/**
 * @brief I2C connection status
 */
typedef enum
{
	I2CMS_OK,								/*!< Status OK */
	I2CMS_TRANSCEIVER_NOT_READY				/*!< Transceiver not ready */
}EI2CSlaveStatus_t;

/**
 * @brief Interface status register
 */
typedef union
{
   uint8_t All;

   struct
	{
       uint8_t LastTransOK    : 1;			/*!< Last transmission state */
	   uint8_t RxDataInBuf    : 1;			/*!< New data received */
	   uint8_t GenAddressCall : 1;			/*!< Main call address served */
	   uint8_t UnusedBits     : 5;			/*!< Unused bits */
	 };
}I2CStatusReg_t;

/**
 * @brief I2C bus configuration
 */
typedef struct
{
	uint8_t Address;						/*!< SLAVE address */
	EI2CSlaveStatus_t ConnStatus;			/*!< I2C Master connection state */
	I2CStatusReg_t Status;					/*!< Interface status */
	EI2CState_t State;						/*!< Current interface state */
}I2CSlave_t;

/* Declaration section -------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/**
 * @brief    Initializes I2C Slave module.
 * @param    i2cConfig : pointer to the configuration structure
 * @retval   None
 */
void I2CSlave_Init(I2CSlave_t* i2cConfig);

/*----------------------------------------------------------------------------*/
/**
 * @brief    Starts transceiver.
 * @param    None
 * @retval   None
 */
void I2CSlave_StartTransceiver();

/*----------------------------------------------------------------------------*/
/**
 * @brief    Deinitializes the I2C module
 * @param    None
 * @retval   None
 */
void I2Cslave_Deinit();

/*----------------------------------------------------------------------------*/
/**
 * @brief    Checks if transceiver is busy
 * @param    None
 * @retval   Busy flag
 */
bool I2CSlave_TransceiverBusy();

/*----------------------------------------------------------------------------*/
/**
 * @brief    Sends data to the I2C Master
 * @param    message: message pointer
 * @param    messageSize: count of message bytes
 * @retval   None
 */
void I2CSlave_SendData(uint8_t* message, uint8_t messageSize);

/*----------------------------------------------------------------------------*/
/**
 * @brief    Reads the interface buffer
 * @param    message: buffer for received data
 * @param    messageSize: bytes count to to be received
 * @retval   None
 */
void I2CSlave_ReadData(uint8_t* message, uint8_t messageSize);

#endif										/* I2C_SLAVE_H */

/******************* (C) COPYRIGHT 2010 HENIUS *************** END OF FILE* ***/
