/**
 *******************************************************************************
 * @file     base_test.h
 * @author   HENIUS (Pawe³ Witak)
 * @version  1.0.1
 * @date     23-04-2020
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

/*! Macro to create test class */
#define TEST_CLASS(className) \
	TEST_CLASS_NAME : public ::testing::Test

/*! Macro to create unit test with fixture class*/
#define UNIT_TEST_F(className, testName)	\
	TEST_F(className, testName)

/*! Macro to create unit test with no fixture class*/
#define UNIT_TEST(testName)	\
	TEST_F(testName##_class, testName)

/*! Macro to get test class name for prametrized test */
#define TEST_CLASS_NAME_WITH_PARAM(testName) \
	testName##_class

/*! Macro to create class for test with params */
#define TEST_CLASS_WITH_PARAM(testName, paramType) \
	TEST_CLASS_NAME_WITH_PARAM(testName) : public ::testing::TestWithParam<paramType>

/*! Macro to create unit test with parameter */
#define UNIT_TEST_WITH_PARAM(testName, ...) \
	INSTANTIATE_TEST_CASE_P( \
			ParameterizedTest, \
			testName##_class, \
			::testing::Values(__VA_ARGS__)); \
	TEST_P(testName##_class, testName)

/******************* (C) COPYRIGHT 2020 HENIUS *************** END OF FILE ****/
