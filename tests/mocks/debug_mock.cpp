/**
 *******************************************************************************
 * @file     debug_mock.h.h
 * @author   HENIUS (Pawe³ Witak)
 * @version  1.0.0
 * @date     01.05.2020
 * @brief    Mock of file debug.h (header file)
 *******************************************************************************
 *
 * <h2><center>COPYRIGHT 2014 HENIUS</center></h2>
 */

/* Include section -----------------------------------------------------------*/

// --->System files

#include <stdio.h>

// --->User files

#include "debug_mock.h"

/* Function section ----------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/*! Mock of function DebugWrite */
void DebugWrite(char *format, ...)
{
    va_list args;

    va_start(args, format);

    char message[100];
    
    vsprintf(message, format, args);
    debug_h_Mock::getInstance().DebugWrite(message);

    va_end(args);
}

/******************* (C) COPYRIGHT 2020 HENIUS *************** END OF FILE ****/
