/**
 *******************************************************************************
 * @file     SerialPort.c
 * @author   HENIUS (Paweł Witak)
 * @version  1.01.003
 * @date     15/11/2013
 * @brief    Serial Port driver with IRQ support
 *******************************************************************************
 *
 * <h2><center>COPYRIGHT 2013 HENIUS</center></h2>
 */

/* Include section -----------------------------------------------------------*/

// --->System files

#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>

// --->User files

#include "SerialPort.h"

/* Variable section ----------------------------------------------------------*/

/*! List wit serial port configurations */
static SerialPort_t SerialPort[SP_NUMBER_OF_PORTS];
/*!< Pointer to the initialization data */
static SPController_t *SerialPortController;

/* Declaration section -------------------------------------------------------*/

/*! Function to send character for printf purpose */
int SerialPort_Transmit(char data, FILE *stream);

/* Function section ----------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/**
 * @brief    Initializes serial port
 * @param    data : pointer to the initialization data
 * @retval   None
 */
void SerialPort_Init(SPController_t *data)
{
	SerialPortController = data;
	if (data->IsPrintfEnabled)
	{
		fdevopen(SerialPort_Transmit, NULL);
	}
	
	// --->Register initialization
	
	// Port SPN_USART0
	SerialPort[SPN_USART0].Register.rUDR = &UDR_0;
	SerialPort[SPN_USART0].Register.rUBRRH = &UBRRH_0;
	SerialPort[SPN_USART0].Register.rUBRRL = &UBRRL_0;
	SerialPort[SPN_USART0].Register.rUCSRA = &UCSRA_0;
	SerialPort[SPN_USART0].Register.rUCSRB = &UCSRB_0;
	SerialPort[SPN_USART0].Register.rUCSRC = &UCSRC_0;
	SerialPort[SPN_USART0].Bit.bTXEN = TXEN_0;
	SerialPort[SPN_USART0].Bit.bRXEN = RXEN_0;
	SerialPort[SPN_USART0].Bit.bUCSZ0 = UCSZ0_0;
	SerialPort[SPN_USART0].Bit.bUCSZ2 = UCSZ2_0;
	SerialPort[SPN_USART0].Bit.bUCPOL = UCPOL_0;
	SerialPort[SPN_USART0].Bit.bU2X = U2X_0;
	SerialPort[SPN_USART0].Bit.bUSBS = USBS_0;
	SerialPort[SPN_USART0].Bit.bUPM0 = UPM0_0;
	SerialPort[SPN_USART0].Bit.bUMSEL = UMSEL_0;
	SerialPort[SPN_USART0].Bit.bRXCIE = RXCIE_0;
	SerialPort[SPN_USART0].Bit.bTXCIE = TXCIE_0;
	SerialPort[SPN_USART0].Bit.bRXC = RXC_0;
	SerialPort[SPN_USART0].Bit.bUDRE = UDRE_0;
	SerialPort[SPN_USART0].Bit.bUDRIE = UDRIE_0;
	
#ifdef SPN_USART1
	// SPN_USART1 port
	SerialPort[SPN_USART1].Register.rUDR = &UDR1;
	SerialPort[SPN_USART1].Register.rUBRRH = &UBRR1H;
	SerialPort[SPN_USART1].Register.rUBRRL = &UBRR1L;
	SerialPort[SPN_USART1].Register.rUCSRA = &UCSR1A;
	SerialPort[SPN_USART1].Register.rUCSRB = &UCSR1B;
	SerialPort[SPN_USART1].Register.rUCSRC = &UCSR1C;
	SerialPort[SPN_USART1].Bit.bTXEN = TXEN1;
	SerialPort[SPN_USART1].Bit.bRXEN = RXEN1;
	SerialPort[SPN_USART1].Bit.bUCSZ0 = UCSZ11;
	SerialPort[SPN_USART1].Bit.bUCSZ2 = UCSZ12;
	SerialPort[SPN_USART1].Bit.bUCPOL = UCPOL1;
	SerialPort[SPN_USART1].Bit.bU2X = U2X1;
	SerialPort[SPN_USART1].Bit.bUSBS = USBS1;
	SerialPort[SPN_USART1].Bit.bUPM0 = UPM01;
	SerialPort[SPN_USART1].Bit.Bit.UMSELbit = UMSEL1;
	SerialPort[SPN_USART1].Bit.bRXCIE = RXCIE1;
	SerialPort[SPN_USART1].Bit.bTXCIE = TXCIE1;
	SerialPort[SPN_USART1].Bit.bRXC = RXC1;
	SerialPort[SPN_USART1].Bit.bUDRE = UDRE1;
	SerialPort[SPN_USART1].Bit.bUDRIE = UDRIE1;
#endif
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Check receive status
 * @param    reg : port register
 * @retval : OPeration status
 */
static ESPRcvStatus_t SerialPort_GetRcvStatus(uint8_t reg)
{
	ESPRcvStatus_t result;

	if (reg & _BV(FE_0))
	{
		result = SPRS_FRAME_ERROR;
	}
	else if (reg & _BV(DOR_0))
	{
		result = SPRS_DATA_OVERRUN_ERROR;
	}
	else if (reg & _BV(UPE_0))
	{
		result = SPRS_PARITY_ERROR;
	}
	else
	{
		result = SPRS_OK;
	}

	return result;
}

/*----------------------------------------------------------------------------*/
void SerialPort_Open(ESPName_t serialPortName, SPDescriptor_t *serialPortConfig)
{
	uint16_t ubrr = 0;
	
	if (IS_SP_EXIST(serialPortName))
	{		
		SerialPort[serialPortName].IsPortOpen = false;
		SerialPort[serialPortName].UsartDescriptor = serialPortConfig;
	
		// --->Data length bits
		if ((serialPortConfig->DataLength - 5) & 0x04)
		{
			*SerialPort[serialPortName].Register.rUCSRB = 
				_BV(SerialPort[serialPortName].Bit.bUCSZ0);
		}
		*SerialPort[serialPortName].Register.rUCSRC = 
			(((serialPortConfig->DataLength - 5) & 0x03) << 
			   SerialPort[serialPortName].Bit.bUCSZ0) |
		// --->Stop bits length
			 ((serialPortConfig->StopBits - 1) << 
			   SerialPort[serialPortName].Bit.bUSBS) |
		// --->Parity
			  (serialPortConfig->Parity << 
			   SerialPort[serialPortName].Bit.bUPM0) |
		// --->Work  mode
			  (serialPortConfig->SyncMode << 
			   SerialPort[serialPortName].Bit.bUMSEL);

		if (serialPortConfig->SyncMode == SPSM_SYNCHRONOUS)
		{
			// --->Ustalanie zbocza sygnału
			*SerialPort[serialPortName].Register.rUCSRC |= 
				(serialPortConfig->Edge << SerialPort[serialPortName].Bit.bUCPOL);
		}

		if (serialPortConfig->SpeedMode == SPSM_DOUBLE)
		{
			// Synchronous mode
			*SerialPort[serialPortName].Register.rUCSRC |= 
				_BV(SerialPort[serialPortName].Bit.bUMSEL);	
			// Double speed mode	
			*SerialPort[serialPortName].Register.rUCSRA |= 
				_BV(SerialPort[serialPortName].Bit.bU2X);
			ubrr = 
				SerialPortController->CpuFrequency / 8 / 
				serialPortConfig->BaudRate - 1;
		}
		else if (serialPortConfig->SpeedMode == SPSM_NORMAL)
		{
			// Asynchronous mode
			*SerialPort[serialPortName].Register.rUCSRC &= 
				~_BV(SerialPort[serialPortName].Bit.bUMSEL); 	
			// Normal speed mode
			*SerialPort[serialPortName].Register.rUCSRA &= 
				~_BV(SerialPort[serialPortName].Bit.bU2X); 	
			ubrr = 
				SerialPortController->CpuFrequency / 16 / 
				serialPortConfig->BaudRate - 1;
		}

		// Speed setting		
		*SerialPort[serialPortName].Register.rUBRRH = (uint8_t) (ubrr >> 8);
		*SerialPort[serialPortName].Register.rUBRRL = (uint8_t) ubrr;

		// Receiver activation
		*SerialPort[serialPortName].Register.rUCSRB |= 
			_BV(SerialPort[serialPortName].Bit.bRXEN) |
		// Transmitter activation
			_BV(SerialPort[serialPortName].Bit.bTXEN);
			
		// IRQ activation
		if (SerialPort[serialPortName].UsartDescriptor->IsIrqEnabled)
		{
			*SerialPort[serialPortName].Register.rUCSRB |=
				_BV(SerialPort[serialPortName].Bit.bRXCIE) |
				_BV(SerialPort[serialPortName].Bit.bUDRIE);
		}
			
		// Port status
		SerialPort[serialPortName].IsPortOpen = true;
	}	
}

/*----------------------------------------------------------------------------*/
void SerialPort_Close(ESPName_t serialPortName)
{
	if (IS_SP_EXIST(serialPortName) &&
	    SerialPort[serialPortName].IsPortOpen)
	{
		*SerialPort[serialPortName].Register.rUCSRB &= ~(
			// Receiver deactivation
			_BV(SerialPort[serialPortName].Bit.bRXEN) | 
			// Transmitter deactivation	
			_BV(SerialPort[serialPortName].Bit.bTXEN)); 		
		SerialPort[serialPortName].IsPortOpen = false;
	}
}

/*----------------------------------------------------------------------------*/
void SerialPort_SendChar(ESPName_t serialPortName, uint8_t character)
{
	if (SerialPort[serialPortName].IsPortOpen &&
	    IS_SP_EXIST(serialPortName) &&
		!SerialPort[serialPortName].UsartDescriptor->IsIrqEnabled)
	{
		// Waiting for ready buffer
		while (!(*SerialPort[serialPortName].Register.rUCSRA &
		       _BV(SerialPort[serialPortName].Bit.bUDRE)));
		
		*SerialPort[serialPortName].Register.rUDR = character;
	}
}

/*----------------------------------------------------------------------------*/
void SerialPort_SendChar_Irq(ESPName_t serialPortName, uint8_t character)
{
	uint8_t tmphead;
	
	if (IS_SP_EXIST(serialPortName) &&
	    SerialPort[serialPortName].UsartDescriptor->IsIrqEnabled)
	{		
		// IRQ activation
		*SerialPort[serialPortName].Register.rUCSRB |=
			_BV(SerialPort[serialPortName].Bit.bUDRIE);
		
		// Buffer index calculation
		tmphead = (SerialPort[serialPortName].TxHead + 1 ) % SP_TX_BUFF_SIZE; 
		// Ready for buffer
		while (tmphead == SerialPort[serialPortName].TxTail);

		SerialPort[serialPortName].TxBuffer[tmphead] = character;           
		SerialPort[serialPortName].TxHead = tmphead;
	}	
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Receives character without IRQ
 * @param    serialPortName : serial port name
 * @param    timeout: receive timeout in ms
 * @retval   Receive character (-1 no data)
 */
int16_t SerialPort_ReceiveChar(ESPName_t serialPortName, uint16_t timeout)
{
	uint8_t udr = -1;
	uint16_t timer = timeout;
	bool isDataReady = false;
	
	if (IS_SP_EXIST(serialPortName) &&
	    !SerialPort[serialPortName].UsartDescriptor->IsIrqEnabled)
	{
		// Waiting for data
		while (!(isDataReady = *SerialPort[serialPortName].Register.rUCSRA & 
			  _BV(SerialPort[serialPortName].Bit.bRXC)) && timer)
		{
			timer--;
			SerialPortController->Delay(1);
		}

		if (isDataReady)
		{
			SerialPort[serialPortName].UsartDescriptor->ReceiveStatus =
				SerialPort_GetRcvStatus(*SerialPort[serialPortName].Register.rUCSRA);
			
			udr = *SerialPort[serialPortName].Register.rUDR; 		
		}
		else
		{
			if (!timer)
			{				
				SerialPort[serialPortName].UsartDescriptor->ReceiveStatus =
					SPRS_TIMEOUT_ERROR;
			}
			
			udr = -1;
		}
	}	

	return udr;
}

/*----------------------------------------------------------------------------*/
int16_t SerialPort_ReceiveChar_Irq(ESPName_t serialPortName, uint16_t timeout)
{
	uint8_t udr = -1;
	uint16_t timer = timeout;
	
	if (IS_SP_EXIST(serialPortName) &&
	    SerialPort[serialPortName].UsartDescriptor->IsIrqEnabled)
	{
		// Waiting for data
		while (!(SerialPort[serialPortName].ReceivedDataLength =
			   SerialPort[serialPortName].RxHead -
			   SerialPort[serialPortName].RxTail) &&
			   timer)
		{
			timer--;
			SerialPortController->Delay(1);
		}
			
		// Checks if data is received.
		if (SerialPort[serialPortName].ReceivedDataLength)
		{
			SerialPort[serialPortName].RxTail = 
				(SerialPort[serialPortName].RxTail + 1 ) % SP_RX_BUFF_SIZE;
				
			udr = SerialPort[serialPortName].
				RxBuffer[SerialPort[serialPortName].RxTail];
			SerialPort[serialPortName].UsartDescriptor->ReceiveStatus =
				SerialPort_GetRcvStatus(
					*SerialPort[serialPortName].Register.rUCSRA);
		}
		else
		{
			if (!timer)
			{
				SerialPort[serialPortName].UsartDescriptor->ReceiveStatus =
					SPRS_TIMEOUT_ERROR;
			}
			
			udr = -1;
		}
	}	

	return udr;
}

/*----------------------------------------------------------------------------*/
uint16_t SerialPort_CountOfBytes(ESPName_t serialPortName)
{
	return SerialPort[serialPortName].ReceivedDataLength;
}

/*----------------------------------------------------------------------------*/
uint8_t* SerialPort_ReadBytes(ESPName_t serialPortName, 
                              uint16_t offset)
{
	return SerialPort[serialPortName].RxBuffer + offset;
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Receive handler
 * @param    serialPortName : serial port name
 * @retval   None
 */
static void SerialPort_ReceiveHandler(ESPName_t serialPortName)
{
	uint8_t data;

	if (IS_SP_EXIST(serialPortName))
	{		
		SerialPort[serialPortName].UsartDescriptor->ReceiveStatus = 
			SerialPort_GetRcvStatus(*SerialPort[serialPortName].Register.rUCSRA);
		data = *SerialPort[serialPortName].Register.rUDR;
		SerialPort[serialPortName].RxHead = 
			(SerialPort[serialPortName].RxHead + 1 ) % SP_RX_BUFF_SIZE;      
		
		if (SerialPort[serialPortName].RxHead == SerialPort[serialPortName].RxTail)
		{
			SerialPort[serialPortName].UsartDescriptor->ReceiveStatus = 
				SPRS_DATA_OVERRUN_ERROR;
		}
	
		SerialPort[serialPortName].RxBuffer[SerialPort[serialPortName].RxHead] = 
			data; 
	}	
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Receive IRQ handler for USART0
 * @param    None
 * @retval   None
 */
ISR(USART0_RX_IRQ)
{
	SerialPort_ReceiveHandler(SPN_USART0);
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Receive IRQ handler for USART1
 * @param    None 
 * @retval   None
 */
#ifdef SPN_USART1
ISR(USART1_RX_IRQ)
{
	SerialPort_ReceiveHandler(SPN_USART1);
}
#endif								/* SPN_USART1 */

/*----------------------------------------------------------------------------*/
/**
 * @brief    Transmit handler
 * @param    serialPortName : serial port name
 * @retval   None
 */
void SerialPort_TransmitHandler(ESPName_t serialPortName)
{
	uint8_t tmptail;

	if (IS_SP_EXIST(serialPortName))
	{
		// Check if all data was sent.
		if (SerialPort[serialPortName].TxHead != 
			SerialPort[serialPortName].TxTail)
		{
			tmptail = (SerialPort[serialPortName].TxTail + 1) % SP_TX_BUFF_SIZE;
			SerialPort[serialPortName].TxTail = tmptail;      
		
			*SerialPort[serialPortName].Register.rUDR = 
				SerialPort[serialPortName].TxBuffer[tmptail]; 
		}
		else
		{
			// IRQ deactivation
			*SerialPort[serialPortName].Register.rUCSRB &= 
				~_BV(SerialPort[serialPortName].Bit.bUDRIE);         
		}
	}	
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    IRQ handler for USART0 transmission
 * @param    None
 * @retval   None
 */
ISR(USART0_TX_IRQ)
{
	SerialPort_TransmitHandler(SPN_USART0);	
}

#ifdef SPN_USART1
ISR(USART1_TX_IRQ)
{
	SerialPort_TransmitHandler(SPN_USART1);
}
#endif								/* SPN_USART1 */

/*----------------------------------------------------------------------------*/
void SerialPort_TransmitChar(ESPName_t serialPortName, uint8_t _char)
{
	if (SerialPort[serialPortName].UsartDescriptor->IsIrqEnabled)
	{
		SerialPort_SendChar_Irq(serialPortName, _char);
	}
	else
	{
		SerialPort_SendChar(serialPortName, _char);
	}
}

/*----------------------------------------------------------------------------*/
void SerialPort_TransmitText(ESPName_t serialPortName, uint8_t* text)
{
	while(*text)
	{
		SerialPort_TransmitChar(serialPortName, *text++);
	}
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    printf support function
 * @param    data : data to send
 * @param    stream : transmitting stream
 * @retval   Function code (0 - success)
 */
int SerialPort_Transmit(char data, FILE* stream)
{
	if (IS_SP_EXIST(SerialPortController->PrintfPort))
	{
		SerialPort_TransmitChar(SerialPortController->PrintfPort,
		                        (uint8_t)data);	
	}	
			
	return 0;
}

/******************* (C) COPYRIGHT 2013 HENIUS *************** END OF FILE ****/
