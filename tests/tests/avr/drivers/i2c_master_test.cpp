/**
 *******************************************************************************
 * @file     i2c_master_test.cpp
 * @author   HENIUS (Pawe³ Witak)
 * @version  1.0.0
 * @date     24-04-2020
 * @brief    Tests of file i2c_master.c
 *******************************************************************************
 *
 * <h2><center>COPYRIGHT 2020 HENIUS</center></h2>
 */

 /* Include section -----------------------------------------------------------*/

 // --->System files

#include <string>
using namespace std;

// --->User files

#include "base_test.h"
#include "debug_mock.h"
#include "i2c_master.c"

/* Declaration section -------------------------------------------------------*/

// --->Test classes

/*! Test class for testing LogI2Cstatus function */
class TEST_CLASS_WITH_PARAM(LogI2CstatusTest, EI2CState_t) { };


/* Function section ----------------------------------------------------------*/

// --->Tests

/*----------------------------------------------------------------------------*/
/**
 * Test of function LogI2Cstatus
 */
UNIT_TEST_WITH_PARAM(
    LogI2CstatusTest,
    I2C_START,
    I2C_REP_START,
    I2C_ARB_LOST,
    I2C_MTX_ADR_ACK,
    I2C_MTX_ADR_NACK,
    I2C_MTX_DATA_ACK,
    I2C_MTX_DATA_NACK,
    I2C_MRX_ADR_ACK,
    I2C_MRX_ADR_NACK,
    I2C_MRX_DATA_ACK,
    I2C_MRX_DATA_NACK,
    I2C_NO_STATE,
    I2C_BUS_ERROR)
{
    auto getErrorMsg = [=](string state) -> string
    {
        return "I2C Error: " + state + "\r\n";
    };
    auto getInfoMsg = [=](string state) -> string
    {
        return "I2C Info: " + state + "\r\n";
    };
    map<EI2CState_t, string> expectedMessages;

    expectedMessages[I2C_BUS_ERROR] = getErrorMsg("I2C_BUS_ERROR");
    expectedMessages[I2C_MTX_DATA_NACK] = getErrorMsg("I2C_MTX_DATA_NACK");
    expectedMessages[I2C_MRX_ADR_NACK] = getErrorMsg("I2C_MRX_ADR_NACK");
    expectedMessages[I2C_MTX_ADR_NACK] = getErrorMsg("I2C_MTX_ADR_NACK");

    expectedMessages[I2C_ARB_LOST] = getInfoMsg("I2C_ARB_LOST");
    expectedMessages[I2C_MRX_DATA_NACK] = getInfoMsg("I2C_MRX_DATA_NACK");
    expectedMessages[I2C_MRX_ADR_ACK] = getInfoMsg("I2C_MRX_ADR_ACK");
    expectedMessages[I2C_MRX_DATA_ACK] = getInfoMsg("I2C_MRX_DATA_ACK");
    expectedMessages[I2C_MTX_DATA_ACK] = getInfoMsg("I2C_MTX_DATA_ACK");
    expectedMessages[I2C_MTX_ADR_ACK] = getInfoMsg("I2C_MTX_ADR_ACK");
    expectedMessages[I2C_REP_START] = getInfoMsg("I2C_REP_START");
    expectedMessages[I2C_START] = getInfoMsg("I2C_START");

    EXPECT_CALL(debug_h_Mock::getInstance(),
                DebugWrite(HasSubstr(expectedMessages[GetParam()])))
        .Times(expectedMessages.find(GetParam()) == expectedMessages.end() ? 0 : 1);

    LogI2Cstatus(GetParam());
}

/******************* (C) COPYRIGHT 2020 HENIUS *************** END OF FILE ****/
