/**
 *******************************************************************************
 * @file     debug_mock.h
 * @author   HENIUS (Pawe³ Witak)
 * @version  1.0.0
 * @date     01.05.2020
 * @brief    Mock of file debug.h (header file)
 *******************************************************************************
 *
 * <h2><center>COPYRIGHT 2014 HENIUS</center></h2>
 */

#pragma once

 /* Include section -----------------------------------------------------------*/

 // --->System files

#include <stdbool.h>
#include <stdint.h>
#include <stdarg.h>
#include <gmock/gmock.h>

using namespace std;

// --->User files

#include "base_mock.h"


/* Macros, constants and definitions section ---------------------------------*/

// --->Constants

#define DebugWrite      DebugWrite_Mock     /*! Mock of function DebugWrite */

// --->Types

/*! Mock class of file debug.h */
class MOCK_CLASS(debug_h_Mock)
{
public:
    MOCK_METHOD(void, DebugWrite, (string));
};

/******************* (C) COPYRIGHT 2020 HENIUS *************** END OF FILE ****/
