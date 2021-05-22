/**
 *******************************************************************************
 * @file     main.cpp
 * @author   HENIUS (Pawe³ Witak)
 * @version  1.0.0
 * @date     22-04-2020
 * @brief    Main program
 *******************************************************************************
 *
 * <h2><center>COPYRIGHT 2020 HENIUS</center></h2>
 */

/* Include section -----------------------------------------------------------*/

// --->System files

#include <gmock/gmock.h>

/* Function section ----------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/**
* @brief    Main function
* @param    None
* @retval   Exit code
*/
int main(int argc, char **argv)
{
    ::testing::InitGoogleMock(&argc, argv);

    return RUN_ALL_TESTS();
}

/******************* (C) COPYRIGHT 2020 HENIUS *************** END OF FILE ****/
