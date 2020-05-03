/**
  ******************************************************************************
  * @file     debug.h
  * @author   HENIUS (Paweł Witak)
  * @version  1.01.001
  * @date     18/03/2011
  * @brief    Implementation of DEBUG support (header file)
  ******************************************************************************
  * 
  * <h2><center>COPYRIGHT 2011 HENIUS</center></h2>
  */

#ifndef DEBUG_H
#define DEBUG_H

/* Include section -----------------------------------------------------------*/

// --->System files

#include <stdbool.h>
#include <avr/io.h>

/* Macros, constants and definitions section ---------------------------------*/

/*! Prints debug message (text in RAM memory). */
#define PRINTF_DBG(msg, ...)	(DebugWrite(msg, __VA_ARGS__))
/*! Prints debug message (text in Flash memory). */
#define PRINTF_DBG_P(msg, ...)	(DebugWrite_P(PSTR(msg), __VA_ARGS__))

/* Declaration section -------------------------------------------------------*/

// --->Functions

/*----------------------------------------------------------------------------*/
/**
 * @brief    Shows Debug message (from RAM)
 * @param    *msg : message
 * @param    ... : args list
 * @retval   None
 */
void DebugWrite(char *msg, ...);

/*----------------------------------------------------------------------------*/
/**
 * @brief    Shows Debug message (from Flash)
 * @param    *msg : message
 * @param    ... : args list
 * @retval   None
 */

void DebugWrite_P(char *msg, ...);

/*----------------------------------------------------------------------------*/
/**
 * @brief    Debug mode activation
 * @param    isDebugSet : true if Debug mode is enabled, otherwise false
 * @retval   None
 */
void SetDebugMode(bool isDebugSet);

#endif								/* DEBUG_H */

/******************* (C) COPYRIGHT 2010 HENIUS *************** KONIEC PLIKU ***/
