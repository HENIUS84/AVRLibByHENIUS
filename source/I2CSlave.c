/**
 *******************************************************************************
 * @file     I2CSlave.c
 * @author   HENIUS (Paweł Witak)
 * @version  1.1.1
 * @date     04/03/2011
 * @brief    I2CSalve driver
 *******************************************************************************
 *
 * <h2><center>COPYRIGHT 2011 HENIUS</center></h2>
 */

/* Include section -----------------------------------------------------------*/

// --->System files

#include <stdio.h>
#include <stdbool.h>
#include <avr/io.h>
#include <stdint.h>
#include <avr/iom32.h>
#include <avr/interrupt.h>

// --->User files

#include "I2CSlave.h"

/* Variable section ----------------------------------------------------------*/

/*! Pointer to the I2C bus configuration */	
static I2CSlave_t* I2CslaveCfg;			
static uint8_t I2CMsgSize;					/*!< Number of sent I2C bytes */
static uint8_t I2Cbuff[I2C_BUFFER_SIZE];	/*!< TX I2C buffer */

/* Function section ----------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
void I2CSlave_Init(I2CSlave_t* i2cConfig)
{
	I2CslaveCfg = i2cConfig;
	TWAR = i2cConfig->Address;

	TWDR = 0xFF;
	TWCR = (1 << TWEN)  |
		   (0 << TWIE)  |
		   (0 << TWINT) |
		   (0 << TWEA)  |
		   (0 << TWSTA) |
		   (0 << TWSTO) |
		   (0 << TWWC);
}

/*----------------------------------------------------------------------------*/
bool I2CSlave_TransceiverBusy()
{
	bool result;

	result = (TWCR & _BV(TWIE)) ? true : false;

	return result;
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Waits for transceiver to be ready.
 * @param    None
 * @retval   None
 */
static void I2Cslave_WaitForTransceiver()
{
	while (I2CSlave_TransceiverBusy());
}

/*----------------------------------------------------------------------------*/
void I2Cslave_Deinit()
{
	TWCR = (0 << TWEN)  |
		   (0 << TWIE)  |
		   (0 << TWINT) |
		   (0 << TWEA)  |
		   (0 << TWSTA) |
		   (0 << TWSTO) |
		   (0 << TWWC);
}

/*----------------------------------------------------------------------------*/
void I2CSlave_SendData(uint8_t* message, uint8_t messageSize)
{
	uint8_t index;

	I2Cslave_WaitForTransceiver();
	I2CMsgSize = messageSize;

	if (messageSize > I2C_BUFFER_SIZE)
	{
		messageSize = I2C_BUFFER_SIZE;
	}

	for (index = 0; index < messageSize; index++)
	{
		I2Cbuff[index] = message[index];
	}

	TWCR = (1 << TWEN)  |
		   (1 << TWIE)  |
		   (1 << TWINT) |
		   (0 << TWSTO) |
		   (1 << TWEA)  |
		   (0 << TWSTA) |
		   (0 << TWWC);
}

/*----------------------------------------------------------------------------*/
void I2CSlave_StartTransceiver()
 {
	 I2Cslave_WaitForTransceiver();
	 TWCR = (1 << TWEN)  |
		    (1 << TWIE)  |
		    (1 << TWINT) |
		    (1 << TWEA)  |
		    (0 << TWSTA) |
		    (0 << TWSTO) |
		    (0 << TWWC);
 }

/*----------------------------------------------------------------------------*/
void I2CSlave_ReadData(uint8_t *message, uint8_t messageSize)
{
	uint8_t index;
	
	I2Cslave_WaitForTransceiver();

	if (I2CslaveCfg->Status.LastTransOK)
	{	
		if (messageSize > I2C_BUFFER_SIZE)
		{
			messageSize = I2C_BUFFER_SIZE;
		}

		for (index = 0; index < messageSize; index++)
		{
			message[index] = I2Cbuff[index];
		}

		I2CslaveCfg->Status.RxDataInBuf = false;
	}
}

/*----------------------------------------------------------------------------*/
/**
 * @brief     IRQ handler
 * @param    None
 * @retval   None
 */
ISR(TWI_vect)
{
	static unsigned char buffIndex;
	
	I2CslaveCfg->State = I2C_STATUS;

	if (I2CslaveCfg->State == I2C_BUS_ERROR)
	{
		I2CslaveCfg->ConnStatus = I2CMS_TRANSCEIVER_NOT_READY;
	}
	else
	{
		I2CslaveCfg->ConnStatus = I2CMS_OK;
	}

	switch(I2CslaveCfg->State)
	{
		// --->Transmitter
		
		// --->Own SLA+R received, ACK sent
		case I2C_STX_ADR_ACK:
			buffIndex   = 0;

		// --->Data from TWDR sent, ACK received
		case I2C_STX_DATA_ACK:
			TWDR = I2Cbuff[buffIndex++];
			TWCR = (1 << TWEN)  |
				   (1 << TWIE)  |
				   (1 << TWINT) |
				   (1 << TWEA)  |
				   (0 << TWSTA) |
				   (0 << TWSTO) |
				   (0 << TWWC);

			break;

		// --->Data from TWDR sent, NACK received (maybe end of transmission)
		case I2C_STX_DATA_NACK:
			if (buffIndex == I2CMsgSize)
			{
				I2CslaveCfg->Status.LastTransOK = true;
			}

			// Transceiver passive mode
			TWCR = (1 << TWEN)  |
			       (0 << TWIE)  |
				   (0 << TWINT) |
				   (0 << TWEA)  |
				   (0 << TWSTA) |
				   (0 << TWSTO) |
				   (0 << TWWC);

			break;

		// --->Receiver
		
		// --->Main call address received, ACK sent
		case I2C_SRX_GEN_ACK:
			I2CslaveCfg->Status.GenAddressCall = true;

		// --->Own SLA+W received, ACK sent
		case I2C_SRX_ADR_ACK:
			I2CslaveCfg->Status.RxDataInBuf = true;
			buffIndex = 0;
			TWCR = (1 << TWEN)  |
				   (1 << TWIE)  |
				   (1 << TWINT) |
				   (1 << TWEA)  |
				   (0 << TWSTA) |
				   (0 << TWSTO) |
				   (0 << TWWC);

			break;

	   // --->Previously addressed SLA+W dane odebrano, ACK sent
	   case I2C_SRX_ADR_DATA_ACK:

	   // --->Previosly address by main call address, data received, ACK sent
	   case I2C_SRX_GEN_DATA_ACK:
	   		I2Cbuff[buffIndex++] = TWDR;
	   		I2CslaveCfg->Status.LastTransOK = true;
	   		TWCR = (1 << TWEN)  |
				   (1 << TWIE)  |
				   (1 << TWINT) |
				   (1 << TWEA)  |
				   (0 << TWSTA) |
				   (0 << TWSTO) |
				   (0 << TWWC);

			  break;

	   // --->STOP condition or repeated START received during addressed Slave
	   case I2C_SRX_STOP_RESTART:
	   		// Passive mode
	   		TWCR = (1 << TWEN)  |
				   (0 << TWIE)  |
				   (0 << TWINT) |
				   (0 << TWEA)  |
				   (0 << TWSTA) |
				   (0 << TWSTO) |
				   (0 << TWWC);

			break;

		// --->Previously addressed SLA+W, data received, NACK sent
		case I2C_SRX_ADR_DATA_NACK:

		// --->Previously addressed by main call address, data received, NACK sent
		case I2C_SRX_GEN_DATA_NACK:

		// --->Last byte sent from TWDR (TWEA = 0), ACK received
		case I2C_STX_DATA_ACK_LAST_BYTE:

		// Errors
		
		// --->Bus error due to improper START/STOP condition
		case I2C_BUS_ERROR:

		// --->Another state
		default:
			// Transmission finished (STOP bit generated), the only why to 
			// obtain bus error.
			TWCR = (1 << TWEN)  |
			       (1 << TWIE)  |
				   (1 << TWINT) |
				   (1 << TWEA)  |
				   (0 << TWSTA) |
				   (1 << TWSTO) |
				   (0 << TWWC);
	}
}

/******************* (C) COPYRIGHT 2010 HENIUS *************** END OF FILE ****/
