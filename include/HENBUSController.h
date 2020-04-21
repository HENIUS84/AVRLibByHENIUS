/**
 *******************************************************************************
 * @file     HENBUSController.h
 * @author   HENIUS (Paweł Witak)
 * @version  1.1.2
 * @date     23-10-2013
 * @brief    Handler of HENBUS protocol (header file)
 *******************************************************************************
 *
 * <h2><center>COPYRIGHT 2013 HENIUS</center></h2>
 */

#ifndef  HENBUS_CONTROLLER_H
#define  HENBUS_CONTROLLER_H

/* Include section------------------------------------------------------------*/

// --->System files

#include <stdbool.h>
#include <stdint.h>

// --->User files

#include "CommunicationController.h"
#include "SerialPort.h"

/* Macros, constants and definitions section ---------------------------------*/

// --->Macros and constants

#define HENBUS_DATA_BUFF_SIZE	(100)	/*!< Receive buffer size */
#define HENBUS_SOF				(':')	/*!< Start of Frame character */
#define HENBUS_EOF              ('#')	/*!< End Of Frame character */
#define HENBUS_TIMEOUT			(4000)	/*!< Receive timeout (in ms) */
#define HENBUS_ASCII_CMD_SIZE	(3)		/*!< Frame size in ASCII mode */

// Fields indexes

#define HENBUS_SOF_LENGTH		(1)			/*! Size of SOF fields */
#define HENBUS_SOF_START_INDEX	(1)			/* Index of beginning of SOF field */
/*! Index of end of SOF field */
#define HENBUS_SOF_END_INDEX	(HENBUS_SOF_START_INDEX + \
                                 HENBUS_SOF_LENGTH - 1)

/*! Address field size */
#ifdef COMM_BINARY_MODE
#define HENBUS_ADDRES_LENGTH	(1)
#else
#define HENBUS_ADDRES_LENGTH	(2)
#endif
/*! Index of beginning of Address field */
#define HENBUS_ADDRESS_START_INDEX		(HENBUS_SOF_END_INDEX + 1)
/*! Index of end of Address field */
#define HENBUS_ADDRESS_END_INDEX		(HENBUS_ADDRESS_START_INDEX + \
                                         HENBUS_ADDRES_LENGTH - 1)
/*! Size of command field */
#ifdef COMM_BINARY_MODE
#define HENBUS_CMD_LENGTH		(1)
#else
#define HENBUS_CMD_LENGTH		(HENBUS_ASCII_CMD_SIZE)
#endif
/*! Index of beginning of Command field */
#define HENBUS_CMD_START_INDEX	(HENBUS_ADDRESS_END_INDEX + 1)
/*! Index of end of Command field */
#define HENBUS_CMD_END_INDEX	(HENBUS_CMD_START_INDEX + \
                                 HENBUS_CMD_LENGTH - 1)

/*! Size of Data field */
#ifdef COMM_BINARY_MODE
#define HENBUS_DATA_SIZE_LENGTH	(1)
#else
#define HENBUS_DATA_SIZE_LENGTH	(2)
#endif
/*! Index of beginning of Data field */
#define HENBUS_DATA_SIZE_START_INDEX	(HENBUS_CMD_END_INDEX + 1)
/*! Index of end of Data field */
#define HENBUS_DATA_SIZE_END_INDEX		(HENBUS_DATA_SIZE_START_INDEX + \
	                                     HENBUS_DATA_SIZE_LENGTH - 1)
										 
/*! Index of beginning of Data or CRC field */
#define HENBUS_DATA_OR_CRC_START_INDEX	(HENBUS_DATA_SIZE_END_INDEX + 1)
/*! Size of CRC field */
#ifdef COMM_BINARY_MODE
#define HENBUS_CRC_LENGTH		(1)
#else
#define HENBUS_CRC_LENGTH		(2)
#endif

/* Declaration section -------------------------------------------------------*/

// --->Functions

/*----------------------------------------------------------------------------*/
/**
* @brief    HENBUS controller initialization
* @param    wdFrame : watchdog frame from PC
* @param    wdAnswerFrame : watchdog answer frame
* @param    serialPortName : serial port name
* @param    frameCallback : frame receive callback
* @param    taskInterval : function repetition interval
* @retval   Structure of controller
*/
CommController_t HENBUSCtrl_Init(const CommProtocolFrame_t* wdTestFrame,
                                 const CommProtocolFrame_t* wdAnswerFrame,
                                 ESPName_t serialPortName,
                                 void (*FrameCallback)(CommProtocolFrame_t*),
                                 uint16_t taskInterval);

#endif								/* HENBUS_CONTROLLER_H */

/******************* (C) COPYRIGHT 2013 HENIUS *************** KONIEC PLIKU ***/