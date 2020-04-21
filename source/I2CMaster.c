/**
 *******************************************************************************
 * @file     I2CMaster.c
 * @author   HENIUS (Paweł Witak)
 * @version  1.1.3
 * @date     03/04/2014
 * @brief    I2C Master driver (based on Atmel'a AVR315 note)
 *******************************************************************************
 *
 * <h2><center>COPYRIGHT 2014 HENIUS</center></h2>
 */

/* Include section -----------------------------------------------------------*/

// --->System files

#include <stdio.h>
#include <avr/pgmspace.h>
#include <avr/io.h>
#include <stdint.h>
#include <stdlib.h>
#include <avr/interrupt.h>

// --->User files

#include "I2CMaster.h"
#include "Debug.h"

/* Variable section ----------------------------------------------------------*/

static I2CMaster_t *I2CMasterCfg;			/*!< I2C configuration pointer */		
static uint8_t I2CMsgSize;					/*!< Bytes size in communication */
static uint8_t I2Cbuff[I2C_BUFFER_SIZE];	/*!< Send buffer */
/*! Buffer for I2C error formatter */
static const uint8_t I2CErrorFormatter[] = "I2C Error %s\r\n";
static I2CStatusReg_t Status;				/*!< Operation status */
static EI2CState_t State;					/*!< Actual state */

/* Function section ----------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
void I2CMaster_Init(I2CMaster_t *i2cConfig)
{	
	int8_t twps = I2C_MAX_OF_TWPS;	
	I2CMasterCfg = i2cConfig;
	State = I2C_NO_STATE;
	Status.All = 0;
				
	// Clock value calculation
	while(twps >= 0)
	{
		TWBR = I2C_GET_TWBR(i2cConfig->CpuFrequency,
		                    i2cConfig->ClockRate,
							twps);
				
		// Max frequency deviation should not be greater than 10kHz		
		if (abs(i2cConfig->ClockRate -
		        I2C_GET_FREQUENCY(i2cConfig->CpuFrequency,
				                  TWBR,
								  twps)) <= I2C_SCK_MAX_ERROR)
		{
			break;
		}
		
		twps--;
	}
	
	// Prescaler setting
	TWSR |= (twps << TWPS0);
	
	Status.IsLastReceivingOk = Status.IsLastSendingOk = false;	
	TWCR = (1 << TWEN)  |
		   (0 << TWIE)  |
		   (0 << TWINT) |
		   (0 << TWSTO) |
		   (0 << TWEA)  |
		   (0 << TWSTA) |
		   (0 << TWWC);
}

/*----------------------------------------------------------------------------*/
bool I2CMaster_IsTransceiverBusy(void)
{
	bool result;

	return (TWCR & _BV(TWIE)) ? true : false;
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Waits for transmitter to be ready
 * @param    None
 * @retval   Operation status (true - transmitter is ready)
 */
static bool I2CMaster_WaitForTransceiver(void)
{
	uint16_t timeoutTimer = I2C_TRANSCEIVER_BUSY_TIMEOUT;
	
	while (I2CMaster_IsTransceiverBusy() && --timeoutTimer);
	
	// Transmitter unlock
	if (!timeoutTimer)
	{
		I2CMaster_Deinit();
	}
	
	return timeoutTimer ? true : false;
}

/*----------------------------------------------------------------------------*/
void I2CMaster_Deinit(void)
{
	TWCR &= ~_BV(TWEN) & ~_BV(TWIE);					
}

/*----------------------------------------------------------------------------*/
bool I2CMaster_SendData(uint8_t* message, uint8_t messageSize)
{
	uint8_t index;
	bool result = I2CMaster_WaitForTransceiver();	

	I2CMsgSize = messageSize + 1;	// Additional byte is SLA+W/R
	I2Cbuff[0] = message[0];

	if (messageSize > I2C_BUFFER_SIZE)
	{
		messageSize = I2C_BUFFER_SIZE;
	}

	if (!(message[0] & I2C_READ_BIT))
	{
		for (index = 1; index < messageSize; index++)
		{
			I2Cbuff[index] = message[index];
		}
	}

	Status.All = 0;
	TWCR = (1 << TWEN)  | 
		   (1 << TWIE)  |
		   (1 << TWINT) | 
		   (0 << TWSTO) |
		   (0 << TWEA)  |
		   (1 << TWSTA) |
		   (0 << TWWC);
		   
	return result;
}

/*----------------------------------------------------------------------------*/
bool I2CMaster_ReadData(uint8_t* message, uint8_t messageSize)
{
	uint8_t index;
	bool result = I2CMaster_WaitForTransceiver();

	if (messageSize > (I2C_BUFFER_SIZE - 1))
	{
		messageSize = I2C_BUFFER_SIZE - 1;
	}

	if (Status.IsLastReceivingOk)
	{
		for (index = 0; index < messageSize; index++)
		{
			message[index] = I2Cbuff[index + 1];
		}
	}
	
	return result;
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Logs I2C status
 * @param    state: I2C communication state
 * @retval   None
 */
static void LogI2Cstatus(EI2CState_t state)
{	
#ifdef I2C_DEBUG_ENABLED
	uint8_t info[] = "I2C Info: %s\r\n";
	uint8_t error[] = "I2C Error: %s\r\n";
	
	switch(state)
	{
		// Normal wok (no issues)

		// START bit sent
		case I2C_START:
			DebugWrite(info, "I2C_START");
			
			break;

		// Double START bit sent
		case I2C_REP_START:
			DebugWrite(info, "I2C_REP_START");
			
			break;

		// SLA+W sent and ACK received
		case I2C_MTX_ADR_ACK:
			DebugWrite(info, "I2C_MTX_ADR_ACK");
			
			break;

		// Data byte sent and ACK received
		case I2C_MTX_DATA_ACK:		
			DebugWrite(info, "I2C_MTX_DATA_ACK");

		// Data byte received and ACK sent
		case I2C_MRX_DATA_ACK:
			DebugWrite(info, "I2C_MRX_DATA_ACK");
			
			break;

		// SLA+R sent and ACK received
		case I2C_MRX_ADR_ACK:
			DebugWrite(info, "I2C_MRX_ADR_ACK");
			
			break;

		// Data byte received and NACK sent
		case I2C_MRX_DATA_NACK:
			DebugWrite(info, "I2C_MRX_DATA_NACK");
			
			break;

		// Arbitration issue (bus error)
		case I2C_ARB_LOST:
			DebugWrite(info, "I2C_ARB_LOST");
			
			break;

		// Error handler

		// SLA+W sent and NACK received
		case I2C_MTX_ADR_NACK:
			DebugWrite(error, "I2C_MTX_ADR_NACK");
			
			break;

		// SLA+R sent and NACK received
		case I2C_MRX_ADR_NACK:
			DebugWrite(error, "I2C_MRX_ADR_NACK");
			
			break;

		// Data byte sent and NACK received
		case I2C_MTX_DATA_NACK:
			DebugWrite(error, "I2C_MTX_DATA_NACK");
			
			break;

		// Bus error due to improper STRT and STOP bits
		case I2C_BUS_ERROR:
			DebugWrite(error, "I2C_BUS_ERROR");
			
			break;
		
		// Anyt other state
		default:		
			break;
	}
#endif								/* I2C_DEBUG_ENABLED */		
}

/*----------------------------------------------------------------------------*/
I2CStatusReg_t I2CMaster_GetStatus()
{
	return Status;
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    I2C interrupt handler
 * @param    None
 * @retval   None
 */
ISR(TWI_vect)
{
	static uint8_t buffIndex;

	State = I2C_STATUS;

	switch(State)
	{
		// Normal work (no issues)

		// --->START bit sent
		case I2C_START:

		// --->Double START bit sent
		case I2C_REP_START:
			buffIndex = 0;

		// --->SLA+W sent and ACK received
		case I2C_MTX_ADR_ACK:

		// --->Data byte sent and ACK received
		case I2C_MTX_DATA_ACK:

			if (buffIndex < I2CMsgSize)
			{
				// Something to send
				Status.IsLastSendingOk = false;
				TWDR = I2Cbuff[buffIndex++];
				TWCR = (1 << TWEN)  |
				       (1 << TWIE)  |
				       (1 << TWINT) |
				       (0 << TWSTO) |
				       (0 << TWEA)  |
				       (0 << TWSTA) |
				       (0 << TWWC);
			}
			else
			{
				// Nothing to send
				Status.IsLastSendingOk = true;
				TWCR = (1 << TWEN)  |
				       (0 << TWIE)  |
				       (1 << TWINT) |
				       (1 << TWSTO) |
				       (0 << TWEA)  |
				       (0 << TWSTA) |
				       (0 << TWWC);
			}

			break;

		// --->Data byte received and ACK sent
		case I2C_MRX_DATA_ACK:
			I2Cbuff[buffIndex++] = TWDR;

		// --->SLA+R sent and ACK received
		case I2C_MRX_ADR_ACK:
			Status.IsLastReceivingOk = false;
		
			if (buffIndex < (I2CMsgSize - 1))
			{
				// This is not previous last byte so send ACK.
				TWCR = (1 << TWEN)  |
					   (1 << TWIE)  |
					   (1 << TWINT) |
					   (0 << TWSTO) |
					   (1 << TWEA)  |
					   (0 << TWSTA) |
					   (0 << TWWC);
			}
			else
			{
				// This is previous last byte (after last byte NACK should be sent).
				TWCR = (1 << TWEN)  |
					   (1 << TWIE)  |
					   (1 << TWINT) |
					   (0 << TWSTO) |
					   (0 << TWEA)  |
					   (0 << TWSTA) |
					   (0 << TWWC);
			}

			break;

		// --->Data byte received and NACK sent
		case I2C_MRX_DATA_NACK:
			// Last byte storing
			I2Cbuff[buffIndex] = TWDR; 

			// Transmission completed (data receiving) :)
			Status.IsLastReceivingOk = true;
			TWCR = (1 << TWEN)  |
				   (0 << TWIE)  |
				   (1 << TWINT) |
				   (1 << TWSTO) |
				   (0 << TWEA)  |
				   (0 << TWSTA) |
				   (0 << TWWC);

			break;

		// --->Arbitration issue (bus error)
		case I2C_ARB_LOST:
			TWCR = (1 << TWEN)  |
				   (1 << TWIE)  |
				   (1 << TWINT) |
				   (0 << TWSTO) |
				   (0 << TWEA)  |
				   (1 << TWSTA) |
				   (0 << TWWC);

			break;

		// Error handler

		// --->SLA+W sent and NACK received
		case I2C_MTX_ADR_NACK:

		// --->SLA+R sent and NACK received
		case I2C_MRX_ADR_NACK:

		// --->Data byte sent and NACK received
		case I2C_MTX_DATA_NACK:

		// --->Bus error due to improper START/STOP bits
		case I2C_BUS_ERROR:
		
		// --->Any other state
		default:
			LogI2Cstatus(State);
			TWCR = (1 << TWEN)  |
				   (0 << TWIE)  |
				   (0 << TWINT) |
				   (0 << TWSTO) |
				   (0 << TWEA)  |
				   (0 << TWSTA) |
				   (0 << TWWC);			
	}
}

/******************* (C) COPYRIGHT 2014 HENIUS *************** END OF FILE ****/
