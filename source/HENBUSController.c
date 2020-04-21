/**
 *******************************************************************************
 * @file     HENBUSController.c
 * @author   HENIUS (Pawe³ Witak)
 * @version  1.1.1
 * @date     23-10-2013
 * @brief    Handler of HENBUS protocol
 *******************************************************************************
 *
 * <h2><center>COPYRIGHT 2013 HENIUS</center></h2>
 */

/* Include section -----------------------------------------------------------*/

// --->System files

#include <stdint.h>
#include <stdio.h>
#include <avr/io.h>
#include <string.h>

// --->User files

#include "HENBUSController.h"
#include "SerialPort.h"
#include "Utils.h"

/* Variable section ----------------------------------------------------------*/

/*! Test frame of Watchdog from PC */
CommProtocolFrame_t WatchdogTestFrame;
/*! Response frame of power supply */
CommProtocolFrame_t WatchdogAnswerFrame;
ESPName_t SerialPortName;			/*! Serial port name */
CommController_t Controller;		/*! Communication controller */
/*! Data buffer */
uint8_t DataBuffer[HENBUS_DATA_BUFF_SIZE];
#ifndef COMM_BINARY_MODE
/*! Table with received command */
uint8_t CurrentCommand[HENBUS_ASCII_CMD_SIZE + 1];
#endif
/*! Currently received frame */
CommProtocolFrame_t CurrentFrame =
{
#ifndef COMM_BINARY_MODE	
	.CommandName = CurrentCommand,
#endif	
	.Data        = DataBuffer
};
/*! Pointer to frame received callback */
void (*FrameReceivedCallback)(CommProtocolFrame_t*);
/*! Timeout (protocol handler repetitions) */
uint16_t TimeoutTime;

/* Function section ----------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/**
* @brief    Sends data in ASCII HEX over serial port
* @param    wdFrame : watchdog frame
* @retval   None
*/
#ifndef COMM_BINARY_MODE
static void HENBUSCtrl_SendAsciHexByte(uint8_t data)
{
	uint8_t asciiHex[3] = { 0, 0 };
	
	ByteToAsciiHex(asciiHex, data);
	SerialPort_TransmitChar(SerialPortName, asciiHex[0]);
	SerialPort_TransmitChar(SerialPortName, asciiHex[1]);
}
#endif

/*----------------------------------------------------------------------------*/
/**
* @brief    Sends frame
* @param    frame : pointer to the frame
* @retval   None
*/
static void HENBUSCtrl_SendFrame(CommProtocolFrame_t* frame)
{
	uint8_t index;
	
	if (frame)
	{
		// Frame sending
		
		// --->SOF - 1 byte
		SerialPort_TransmitChar(SerialPortName, HENBUS_SOF);
		
		// --->Device address
#ifdef COMM_BINARY_MODE
		// BINARY mode - 1 byte
		SerialPort_TransmitChar(SerialPortName, frame->Address);
#else		
		// ASCII mode - 2 bytes
		HENBUSCtrl_SendAsciHexByte(frame->Address);
#endif
		
		// --->Command code
#ifdef COMM_BINARY_MODE
		// BINARY mode - 1 byte
		SerialPort_TransmitChar(SerialPortName, frame->CommandID);
#else
		// ASCII mode - variable bytes
		SerialPort_TransmitText(SerialPortName, frame->CommandName);
#endif
		
		// --->Data size
#ifdef COMM_BINARY_MODE
		// BINARY mode - 1 byte
		SerialPort_TransmitChar(SerialPortName, frame->DataSize);
#else
		// ASCII mode - 2 bytes
		HENBUSCtrl_SendAsciHexByte(frame->DataSize);
#endif
		
		// --->Data field
		if (frame->DataSize > 0)
		{
			// Data sending - frame->DataSize * 2 bytes
			for (index = 0; index < frame->DataSize; index++)
			{
#ifdef COMM_BINARY_MODE
				// BINARY mode - 1 byte * DataSize
				SerialPort_TransmitChar(SerialPortName, frame->Data[index]);
#else
				// ASCII mode - 2 bytes * DataSize
				HENBUSCtrl_SendAsciHexByte(frame->Data[index]);
#endif
			}
			
			// --->CRC - 2 bytes
#ifdef COMM_BINARY_MODE
			// BINARY mode - 1 byte
			SerialPort_TransmitChar(
				SerialPortName,
				CRC8(frame->Data, frame->DataSize));
#else
			// ASCII mode - 2 bytes
			HENBUSCtrl_SendAsciHexByte(CRC8(frame->Data, frame->DataSize));
#endif
		}
		
		// --->EOF - 1 byte
		SerialPort_TransmitChar(SerialPortName, HENBUS_EOF);
	}
}

/*----------------------------------------------------------------------------*/
/**
* @brief    HENBUScontroller handler
* @param    None
* @retval   Connection state (true - connected)
*/
static bool HENBUSCtrl_Handler()
{
	// Currently received byte
	uint8_t currentByte = SerialPort_ReceiveChar_Irq(SerialPortName, 0);	
	static uint8_t byteIdx = 0;		// Bytes index
#ifndef COMM_BINARY_MODE
	static uint8_t AsciiHexByte[2];	// Current ASCII HEX byte
#endif
	// Beginning and end index of current field
	static int8_t currentFieldStartIndex = 0, currentFieldEndIndex = 0;
	// Beginning and end index of data field
	static uint8_t dataStartIndex = 0, dataEndIndex = 0;
	// Beginning and end index of CRC field
	static uint8_t crcStartIndex = 0, crcEndIndex = 0;	
	static uint8_t crcOfFrame = 0;			// CRC of current frame	
	static uint16_t timeoutTimer = 1;		// Timeout timer	
	static bool isConnected = false;		// Flag of connection status
	
	if (!--timeoutTimer)
	{
		isConnected = false;
		timeoutTimer = TimeoutTime;
	}
					
	// --->Bytes analysis
	
	// Save of frame byte by byte
	if(currentByte == HENBUS_SOF ||
	   currentByte == HENBUS_EOF ||
	   (currentByte >= 0 && byteIdx))
	{
		// SOF detection
		if (currentByte == HENBUS_SOF)
		{
			// Frame receive initialization
			CurrentFrame.Address = 0;
			CurrentFrame.DataSize = 0;			
			byteIdx = HENBUS_SOF_START_INDEX;
			
			// Indexes of Address field
			currentFieldStartIndex = HENBUS_ADDRESS_START_INDEX;
			currentFieldEndIndex = HENBUS_ADDRESS_END_INDEX;
		}
		
		// Completing the frame 
		if (byteIdx >= currentFieldStartIndex &&
		    byteIdx <= currentFieldEndIndex)
		{
			// What is this field?
								
			if (// --->Address field
				currentFieldStartIndex == HENBUS_ADDRESS_START_INDEX ||
				// --->Data size field
				currentFieldStartIndex == HENBUS_DATA_SIZE_START_INDEX ||
				// --->CRC field
				currentFieldStartIndex == crcStartIndex)
			{
#ifndef COMM_BINARY_MODE
				AsciiHexByte[byteIdx - currentFieldStartIndex] = 
					currentByte;
#endif					
					
				// Is it last character?
				if (byteIdx == currentFieldEndIndex)
				{
					// What field?
					
					// --->Address field
					if  (currentFieldStartIndex == HENBUS_ADDRESS_START_INDEX)
					{						
						CurrentFrame.Address =
#ifndef COMM_BINARY_MODE						
							AsciiHexToByte(AsciiHexByte);
#else
							currentByte;
#endif
								
						// Command field indexes
						currentFieldStartIndex = HENBUS_CMD_START_INDEX;
						currentFieldEndIndex = HENBUS_CMD_END_INDEX;
					} else
					// --->Field of data size
					if (currentFieldStartIndex == HENBUS_DATA_SIZE_START_INDEX)
					{		
						CurrentFrame.DataSize =
#ifndef COMM_BINARY_MODE						
							AsciiHexToByte(AsciiHexByte);
#else
							currentByte;
#endif							
											
						// Indexes of data and CRC field					
						if (CurrentFrame.DataSize)
						{										
							currentFieldStartIndex = dataStartIndex =
								HENBUS_DATA_OR_CRC_START_INDEX;
							currentFieldEndIndex = dataEndIndex =
								HENBUS_DATA_OR_CRC_START_INDEX + 
								CurrentFrame.DataSize
#ifndef COMM_BINARY_MODE
								* 2
#endif								
								- 1;
							crcStartIndex = dataEndIndex + 1;
							crcEndIndex = crcStartIndex + HENBUS_CRC_LENGTH - 1;
						} 
						else
						{
							// End of transmission if no more bytes in frame 
							currentFieldStartIndex = currentFieldEndIndex = 0;
						}
					} else
					// --->CRC field
					if (currentFieldStartIndex == crcStartIndex)
					{
						crcOfFrame =
#ifndef COMM_BINARY_MODE						
							AsciiHexToByte(AsciiHexByte);
#else
							currentByte;	
#endif												
					}
				}
			}			
			else
			// --->Command field
		    if (currentFieldStartIndex == HENBUS_CMD_START_INDEX)
			{
#ifndef COMM_BINARY_MODE				
				CurrentFrame.CommandName[byteIdx - currentFieldStartIndex] =
					currentByte;
				CurrentFrame.CommandName[byteIdx - currentFieldStartIndex + 1] =
					0;
#endif					
						
				// Is it last byte of Command field?
				if (byteIdx == currentFieldEndIndex)
				{
#ifdef COMM_BINARY_MODE
					CurrentFrame.CommandID = currentByte;
#endif					
					
					// Indexes of data size field
					currentFieldStartIndex = HENBUS_DATA_SIZE_START_INDEX;
					currentFieldEndIndex = HENBUS_DATA_SIZE_END_INDEX;
				}
				
			}
			else
			// --->Data field
			if (currentFieldStartIndex == dataStartIndex)
			{
#ifndef COMM_BINARY_MODE				
				AsciiHexByte[(byteIdx - currentFieldStartIndex) % 2] =
					currentByte;
					
				// Is id complete byte?
				if ((byteIdx - currentFieldStartIndex) % 2 == 1)
				{
					CurrentFrame.Data[(byteIdx - currentFieldStartIndex) / 2] =
						AsciiHexToByte(AsciiHexByte);
				}
#else
				CurrentFrame.Data[byteIdx - currentFieldStartIndex] =
					currentByte;			
#endif				
				
				// Is it end of data?
				if (byteIdx == currentFieldEndIndex)
				{
					// Indexes of CRC field
					currentFieldStartIndex = currentFieldEndIndex + 1;
					currentFieldEndIndex = currentFieldStartIndex + 
						HENBUS_CRC_LENGTH  - 1;
				}
			}
			
		}	
			
		byteIdx++;
		
		// Do we have complete frame?
		if (currentByte == HENBUS_EOF)
		{
			byteIdx = 0;
			
			// CRC check
			if ((CRC8(CurrentFrame.Data, CurrentFrame.DataSize) == crcOfFrame ||
			    !CurrentFrame.DataSize) &&
			    FrameReceivedCallback)
			{
				// Timer reset
				timeoutTimer = TimeoutTime;
				isConnected = true;
				
				// Do we have complete Watchdog frame?
				if (
#ifndef COMM_BINARY_MODE				
					!strcmp((char*)CurrentFrame.CommandName,
				            (char*)WatchdogTestFrame.CommandName))
#else
					CurrentFrame.CommandID == WatchdogTestFrame.CommandID)
#endif							
				{
					HENBUSCtrl_SendFrame(&WatchdogAnswerFrame);
				}
				
				FrameReceivedCallback(&CurrentFrame);				
			}
		}
	}
	
	return isConnected;
}

/*----------------------------------------------------------------------------*/
CommController_t HENBUSCtrl_Init(const CommProtocolFrame_t* wdTestFrame,
                                 const CommProtocolFrame_t* wdAnswerFrame,
					             ESPName_t serialPortName,
				                 void (*frameCallback)(CommProtocolFrame_t*),
								 uint16_t taskInterval) 
{
	WatchdogTestFrame = *wdTestFrame;
	WatchdogAnswerFrame = *wdAnswerFrame;
	SerialPortName = serialPortName;
	TimeoutTime = HENBUS_TIMEOUT / taskInterval;
	Controller.SendFrame = HENBUSCtrl_SendFrame;
	Controller.Handler = HENBUSCtrl_Handler;
	FrameReceivedCallback = frameCallback;
	
	return Controller;
}

/******************* (C) COPYRIGHT 2013 HENIUS *************** END OF FILE ****/
