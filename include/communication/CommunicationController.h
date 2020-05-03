/**
 *******************************************************************************
 * @file     CommunicationController.h
 * @author   HENIUS (Paweł Witak)
 * @version  1.1.2
 * @date     09-11-2013
 * @brief    Communication protocols handler (header file)
 *******************************************************************************
 *
 * <h2><center>COPYRIGHT 2013 HENIUS</center></h2>
 */

#ifndef  COMMUNICATION_CONTROLLER_H
#define  COMMUNICATION_CONTROLLER_H

/* Include section -----------------------------------------------------------*/

// --->System files

#include <stdbool.h>
#include <stdint.h>

/* Declaration section -------------------------------------------------------*/

// --->Constants

#define COMM_BINARY_MODE					/*! Binary mode */

// --->Types

/**
 * @brief Structure of protocol frame
 */
typedef struct  
{
	uint8_t Address;						/*!< Devic eaddress */
#ifdef COMM_BINARY_MODE
	uint8_t CommandID;						/*!< Command ID */
#else
	uint8_t* CommandName;					/*!< Command name */	
#endif
	/*! Count of data bytes (after encoding) */
	uint8_t DataSize;
	uint8_t* Data;							/*!< Data buffer */
}CommProtocolFrame_t;

/**
 * @brief Communication controller structure
 */
typedef struct
{
	/*! Pointer to the frame sending function */
	void(*SendFrame)(CommProtocolFrame_t* frame);
	/*! Pointer to the controller handler function */
	bool(*Handler)(void);	
}CommController_t;

#endif								/* COMMUNICATION_CONTROLLER_H */

/******************* (C) COPYRIGHT 2013 HENIUS *************** END OF FILE ****/