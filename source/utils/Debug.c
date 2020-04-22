/**
 *******************************************************************************
 * @file     Debug.c
 * @author   HENIUS (Paweł Witak)
 * @version  1.1.1
 * @date     18/03/2011
 * @brief    Implementation of DEBUG support
 *******************************************************************************
 *
 * <h2><center>COPYRIGHT 2011 HENIUS</center></h2>
 */

/* Include section -----------------------------------------------------------*/

// --->System files

#include <stdint.h>
#include <stdio.h>

// --->User files

#include "Debug.h"

/* Variable section ----------------------------------------------------------*/

static bool DbgModeEnabled = false;			/*!< Debug mode activation */
static uint8_t TextBuffer[100];				/* Text buffer */

/* Function section ----------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
void SetDebugMode(bool isDebugSet)
{
	DbgModeEnabled = isDebugSet;
}

/*----------------------------------------------------------------------------*/
void DebugWrite(char *msg, ...)
{
	va_list args;
	
	if (DbgModeEnabled)
	{
		va_start(args, msg);
		vsprintf((char*)TextBuffer, msg, args); 
		printf("%s", (char*)TextBuffer);
		va_end(args);
	}
}

/*----------------------------------------------------------------------------*/
void DebugWrite_P(char *msg, ...)
{
	va_list args;

	if (DbgModeEnabled)
	{
		va_start(args, msg);
		vsprintf_P((char*)TextBuffer, msg, args);
		printf("%s", TextBuffer);
		va_end(args);
	}
}

/******************* (C) COPYRIGHT 2011 HENIUS *************** END OF FILE ****/
