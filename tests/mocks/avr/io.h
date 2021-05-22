/**
 *******************************************************************************
 * @file     io.h
 * @author   HENIUS (Pawe³ Witak)
 * @version  1.0.0
 * @date     24-04-2020
 * @brief    Mock of <avr/io.h> file (header file)
 *******************************************************************************
 *
 * <h2><center>COPYRIGHT 2020 HENIUS</center></h2>
 */

#pragma once

/* Macros, constants and definitions section ---------------------------------*/

#define _BV(bitPos)     (1 << bitPos)

/* 2-wire Status Register - TWCR */
#define  TWINT          7
#define  TWEA           6
#define  TWSTA          5
#define  TWSTO          4
#define  TWWC           3
#define  TWEN           2
#define  TWIE           0

/* 2-wire Status Register - TWSR */
#define  TWS7           7
#define  TWS6           6
#define  TWS5           5
#define  TWS4           4
#define  TWS3           3
#define  TWPS1          1
#define  TWPS0          0

// Registers

int TWSR;                                   /*! Register TWSR */
int TWCR;                                   /*! Register TWCR */
int TWBR;                                   /*! Register TWBR */
int TWDR;                                   /*! Register TWDR */

/******************* (C) COPYRIGHT 2020 HENIUS *************** END OF FILE ****/
