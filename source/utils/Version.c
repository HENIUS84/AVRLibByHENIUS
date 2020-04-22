/**
 *******************************************************************************
 * @file     Version.c
 * @author   HENIUS (Paweł Witak) 
 * @version  1.1.2
 * @date     2013-04-18
 * @brief    Service of firmware version support
 *************************************************************************************
 *
 * <h2><center>COPYRIGHT 2013 HENIUS</center></h2>
 */

/* Include section -----------------------------------------------------------*/

// --->User files

#include "Version.h"

// --->System files

#include <stdlib.h>

/* Function section ----------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
uint8_t GetCompileHour(void)
{
  return COMPILE_HOUR;
}

/*----------------------------------------------------------------------------*/
uint8_t GetCompileMinute(void)
{
  return COMPILE_MINUTE;
}

/*----------------------------------------------------------------------------*/
uint8_t GetCompileSecond(void)
{
  return COMPILE_SECOND;
}

/*----------------------------------------------------------------------------*/
uint16_t GetCompileYear(void)
{
  return COMPILE_YEAR;
}

/*----------------------------------------------------------------------------*/
uint8_t GetCompileMonth(void)
{
  return COMPILE_MONTH;
}

/*----------------------------------------------------------------------------*/
uint8_t GetCompileDay(void)
{
  return COMPILE_DAY;
}

/*----------------------------------------------------------------------------*/
void GetFirmwareVersion(FirmwareInfo_t *version, uint8_t *versionString)
{
	uint8_t tempBuff[10];
	uint8_t index;
	
	version->CompileDay = GetCompileDay();
	version->CompileMonth = GetCompileMonth();
	version->CompileYear = GetCompileYear();
	version->CompileHour = GetCompileHour();
	version->CompileMinute = GetCompileMinute();
	
	// Major
	index = 0;
	while (*versionString && *versionString != '.')
	{
		tempBuff[index++] = *versionString++;
		tempBuff[index] = 0;
	}	
	version->MajorVersion = atoi((char*)tempBuff);
	
	// Minor
	index = 0;
	versionString++;
	while (*versionString && *versionString != '.')
	{
		tempBuff[index++] = *versionString++;
		tempBuff[index] = 0;
	}
	version->MinorVersion = atoi((char*)tempBuff);
	
	// Revision
	index = 0;
	versionString++;
	while (*versionString && *versionString != '.')
	{
		tempBuff[index++] = *versionString++;
		tempBuff[index] = 0;
	}
	version->Revision = atoi((char*)tempBuff);
}

/******************* (C) COPYRIGHT 2013 HENIUS *************** END OF FILE ****/
