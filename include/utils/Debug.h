/**
  ******************************************************************************
  * @file     Debug.h
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

/*! Wyświetlanie komunikatu debug (umieszcza tekst w pamięci RAM) */
#define PRINTF_DBG(msg, args...)	(DebugWrite(msg, ##args))
/*! Wyświetlanie komunikatu debug (umieszcza tekst w pamięci programu) */
#define PRINTF_DBG_P(msg, args...)	(DebugWrite_P(PSTR(msg), ##args))

/* Declaration section -------------------------------------------------------*/

// ---> Funkcje

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
