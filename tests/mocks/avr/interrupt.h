/**
 *******************************************************************************
 * @file     interrupt.h
 * @author   HENIUS (Pawe³ Witak)
 * @version  1.0.0
 * @date     24-04-2020
 * @brief    Mock of <avr/interrupt.h> file (header file)
 *******************************************************************************
 *
 * <h2><center>COPYRIGHT 2020 HENIUS</center></h2>
 */

#pragma once

 /* Macros, constants and definitions section ---------------------------------*/

/*!< Mock macro for interrupt handler */
#define ISR(vector)     static void vector()

/******************* (C) COPYRIGHT 2020 HENIUS *************** END OF FILE ****/
