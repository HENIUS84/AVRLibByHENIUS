/**
 *******************************************************************************
 * @file     utils_test.cpp
 * @author   HENIUS (Pawe³ Witak)
 * @version  1.0.0
 * @date     22-04-2020
 * @brief    Tests of file utils.c
 *******************************************************************************
 *
 * <h2><center>COPYRIGHT 2020 HENIUS</center></h2>
 */

/* Include section -----------------------------------------------------------*/

// --->System files

#include <string>
#include <sstream>
#include <iostream>
using namespace std;

// --->User files

#include "Utils.c"
#include "base_test.h"

/* Declaration section -------------------------------------------------------*/

// --->Test classes

/*! Test class for testing AsciiHexToByte function */
class TEST_CLASS_WITH_PARAM(AsciiHexToByteTest, string) { };

/*! Test class for testing ByteToAsciiHex function */
class TEST_CLASS_WITH_PARAM(ByteToAsciiHexTest, uint8_t) { };

/* Function section ----------------------------------------------------------*/

// --->Tests

/*----------------------------------------------------------------------------*/
/**
 * Test of function AsciiHexToByte
 */
UNIT_TEST_WITH_PARAM(AsciiHexToByteTest, "00", "11", "AA", "BB", "FF")
{
	string asciiHexStr = GetParam();
	int expectedByte;
	stringstream hexStream;

	hexStream << hex << asciiHexStr;
	hexStream >> expectedByte;

	EXPECT_EQ(AsciiHexToByte((uint8_t *)asciiHexStr.c_str()), expectedByte);
}

/*----------------------------------------------------------------------------*/
/**
 * Test of function ByteToAsciiHex
 */
UNIT_TEST_WITH_PARAM(ByteToAsciiHexTest, 0x00, 0x22, 0xAA, 0xFF)
{
	int testedValue = GetParam();
	stringstream  hexStream;
	uint8_t resultAsciiHex[100];

	hexStream << hex << setfill('0') << setw(2) << uppercase << testedValue;
	string expectedAsciiHex = hexStream.str();

	ByteToAsciiHex(resultAsciiHex, GetParam());

	EXPECT_STREQ(reinterpret_cast<char *>(resultAsciiHex),
			     expectedAsciiHex.c_str());
}

/******************* (C) COPYRIGHT 2020 HENIUS *************** END OF FILE ****/
