/**
 *******************************************************************************
 * @file     base_test.h
 * @author   HENIUS (Pawe³ Witak)
 * @version  1.0.0
 * @date     22-04-2020
 * @brief    Extensions for gtest library (header file)
 *******************************************************************************
 *
 * <h2><center>COPYRIGHT 2020 HENIUS</center></h2>
 */

#pragma once

/* Include section -----------------------------------------------------------*/

// --->System files

#include <gtest/gtest.h>

/* Macros, constants and definitions section ---------------------------------*/

#ifndef TEST_CLASS_NAME
	#error "Test class name is not specified"
#endif

/*! Macro to create test class */
#define TEST_CLASS()  		class TEST_CLASS_NAME : public ::testing::Test

/*! Macro to create unit test */
#define UNIT_TEST(testName)	TEST_F(TEST_CLASS_NAME, testName)

/******************* (C) COPYRIGHT 2020 HENIUS *************** END OF FILE ****/
