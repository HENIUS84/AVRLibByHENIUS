/**
 *******************************************************************************
 * @file     Version.h                                                      
 * @author   HENIUS (Paweł Witak)                                              
 * @version  1.01.002                                                          
 * @date     2013-04-18                                             
 * @brief    Service of firmware version support (header file)                                 
 *******************************************************************************
 *
 * <h2><center>COPYRIGHT 2013 HENIUS</center></h2>
 */

#ifndef VERSION_H_
#define VERSION_H_

/* Include section -----------------------------------------------------------*/

// --->System files

#include <stdint.h>
#include <stdbool.h>

/* Macros, constants and definitions section ---------------------------------*/

// --->Macros for firmware version support

/*! Gets the build hour */
#define COMPILE_HOUR		(((__TIME__[0] - '0') * 10) + \
                               __TIME__[1] - '0')
/*! Gets the build minutes */
#define COMPILE_MINUTE		(((__TIME__[3] - '0') * 10) + \
                               __TIME__[4] - '0')
/*! Gets the build seconds */
#define COMPILE_SECOND		(((__TIME__[6] - '0') * 10) + \
                              (__TIME__[7] - '0'))
/*! Gets the build year */
#define COMPILE_YEAR		((((__DATE__[7] - '0') * 10 + \
                               (__DATE__[8] - '0')) * 10 + \
                               (__DATE__[9] - '0')) * 10 + \
						       (__DATE__[10] - '0'))
/*! Gets the build month (0-11) */
#define COMPILE_MONTH		(__DATE__[2] == 'n' ? (__DATE__[1] == 'a' ? 1 : 6) \
                           : __DATE__[2] == 'b' ? 2 \
						   : __DATE__[2] == 'r' ? (__DATE__[0] == 'M' ? 3 : 4) \
                           : __DATE__[2] == 'y' ? 5 \
                           : __DATE__[2] == 'l' ? 7 \
                           : __DATE__[2] == 'g' ? 8 \
                           : __DATE__[2] == 'p' ? 9 \
                           : __DATE__[2] == 't' ? 10 \
                           : __DATE__[2] == 'v' ? 11 : 12)
/*! Gets the build date */
#define COMPILE_DAY			((__DATE__[4]== ' ' ? 0 : __DATE__[4] - '0') * 10 + \
                             (__DATE__[5] - '0'))
							 
// --->Types

/**
 * @brief Firmware version information
 */
typedef struct
{
	
	uint16_t CompileDay    : 5;				/*!< Build day */	
	uint16_t CompileMonth  : 4;				/*!< Build month */	
	uint16_t CompileYear   : 12;			/*!< Build year */
	uint16_t CompileHour   : 5;				/*!< Build hour */
	uint16_t CompileMinute : 6;				/*!< Build minutes */
	uint16_t MajorVersion  : 3;				/*!< Major version */
	uint16_t MinorVersion  : 7;				/*!< Minor version */
	uint16_t Revision      : 10;			/*!< Revision number */
}FirmwareInfo_t;

/* Declaration section -------------------------------------------------------*/

// --->Functions

/*----------------------------------------------------------------------------*/
/**
 * @brief    Gets the build hour
 * @param    None
 * @retval   Build hour
 */
uint8_t GetCompileHour(void);

/*----------------------------------------------------------------------------*/
/**
 * @brief    Gets the build minutes
 * @param    None
 * @retval   Build minutes
 */
uint8_t GetCompileMinute(void);

/*----------------------------------------------------------------------------*/
/**
 * @brief    Gets the build seconds
 * @param    None
 * @retval   Build seconds
 */
uint8_t GetCompileSecond(void);

/*----------------------------------------------------------------------------*/
/**
 * @brief    Gets the build year
 * @param    None
 * @retval   Build year
 */
uint16_t GetCompileYear(void);

/*----------------------------------------------------------------------------*/
/**
 * @brief    Gets the build month
 * @param    None
 * @retval   Build month
 */
uint8_t GetCompileMonth(void);

/*----------------------------------------------------------------------------*/
/**
 * @brief    Gets the build day
 * @param    None
 * @retval   Build day
 */
uint8_t GetCompileDay(void);

/*----------------------------------------------------------------------------*/
/**
 * @brief    Gets the version and build date of firmware
 * @param    *version : pointer to the firmware information structure
 * @param    *versionString : text string with firmware version 
 *           (in format Major Version.Minor Version.Revision)
 * @retval   None
 */
void GetFirmwareVersion(FirmwareInfo_t *version, uint8_t *versionString);

#endif								/* VERSION_H_  */

/******************* (C) COPYRIGHT 2013 HENIUS *************** END OF FILE* ***/
