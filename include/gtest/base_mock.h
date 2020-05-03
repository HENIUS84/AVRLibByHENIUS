/**
 *******************************************************************************
 * @file     base_mock.h
 * @author   HENIUS (Pawe³ Witak)
 * @version  1.0.0
 * @date     30-04-2020
 * @brief    Extensions for gmock library (header file)
 *******************************************************************************
 *
 * <h2><center>COPYRIGHT 2020 HENIUS</center></h2>
 */

#pragma once

/* Include section -----------------------------------------------------------*/

// --->System files

/* Macros, constants and definitions section ---------------------------------*/

/*! Macro to create test class */
#define MOCK_CLASS(className) \
	className : public BaseMock<className>


/*! Base class of mock */
template<typename T>
class BaseMock
{
public:
    static T& getInstance()
    {
        static T instance;
        return instance;
    }
};

/******************* (C) COPYRIGHT 2020 HENIUS *************** END OF FILE ****/
