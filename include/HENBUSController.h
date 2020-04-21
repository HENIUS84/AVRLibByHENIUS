/**
 *******************************************************************************
 * @file     HENBUSController.h
 * @author   HENIUS (Paweł Witak)
 * @version  1.1.2
 * @date     23-10-2013
 * @brief    Obsługa transmisji po protokole HENBUS (plik nagłówkowy)
 *******************************************************************************
 *
 * <h2><center>COPYRIGHT 2013 HENIUS</center></h2>
 */

#ifndef  HENBUS_CONTROLLER_H
#define  HENBUS_CONTROLLER_H

/* Sekcja include ------------------------------------------------------------*/

// --->Pliki systemowe

#include <stdbool.h>
#include <stdint.h>

// --->Pliki użytkownika

#include "CommunicationController.h"
#include "SerialPort.h"

/* Sekcja stałych, makr i definicji ------------------------------------------*/

// --->Stałe i makra

#define HENBUS_DATA_BUFF_SIZE	(100)	/*!< Rozmiar bufora odbiorczego */
#define HENBUS_SOF				(':')	/*!< Znak początku ramki */
#define HENBUS_EOF              ('#')	/*!< Znak końca ramki */
#define HENBUS_TIMEOUT			(4000)	/*!< Timeout odbioru (w ms) */
#define HENBUS_ASCII_CMD_SIZE	(3)		/*!< Rozmiar komendy ramki w 
                                             trybie ASCII*/

// Indeksy pól

/*! Rozmiar znacznika początku */
#define HENBUS_SOF_LENGTH				(1)
/* Indeks początku znacznika początku ramki */
#define HENBUS_SOF_START_INDEX			(1)
/*! Indeks końca znacznika początku ramki */
#define HENBUS_SOF_END_INDEX			(HENBUS_SOF_START_INDEX + \
                                         HENBUS_SOF_LENGTH - 1)
/*! Rozmiar pola adresu */
#ifdef COMM_BINARY_MODE
#define HENBUS_ADDRES_LENGTH			(1)
#else
#define HENBUS_ADDRES_LENGTH			(2)
#endif
/*! Indeks początku pola adresu */
#define HENBUS_ADDRESS_START_INDEX		(HENBUS_SOF_END_INDEX + 1)
/*! Indeks końca pola adresu */
#define HENBUS_ADDRESS_END_INDEX        (HENBUS_ADDRESS_START_INDEX + \
                                         HENBUS_ADDRES_LENGTH - 1)
/*! Rozmiar pola komendy */
#ifdef COMM_BINARY_MODE
#define HENBUS_CMD_LENGTH				(1)
#else
#define HENBUS_CMD_LENGTH				(HENBUS_ASCII_CMD_SIZE)
#endif
/*! Indeks początku pola komendy */
#define HENBUS_CMD_START_INDEX			(HENBUS_ADDRESS_END_INDEX + 1)
/*! Indeks końca pola komendy */
#define HENBUS_CMD_END_INDEX			(HENBUS_CMD_START_INDEX + \
                                         HENBUS_CMD_LENGTH - 1)
/*! Rozmiar pola rozmiaru danych */
#ifdef COMM_BINARY_MODE
#define HENBUS_DATA_SIZE_LENGTH			(1)
#else
#define HENBUS_DATA_SIZE_LENGTH			(2)
#endif
/*! Indeks początku pola rozmiaru danych */
#define HENBUS_DATA_SIZE_START_INDEX	(HENBUS_CMD_END_INDEX + 1)
/*! Indeks końca pola rozmiaru danych */
#define HENBUS_DATA_SIZE_END_INDEX		(HENBUS_DATA_SIZE_START_INDEX + \
	                                     HENBUS_DATA_SIZE_LENGTH - 1)
/*! Indeks początku pola danych lub CRC */
#define HENBUS_DATA_OR_CRC_START_INDEX	(HENBUS_DATA_SIZE_END_INDEX + 1)
/*! Rozmiar pola sumy kontrolnej */
#ifdef COMM_BINARY_MODE
#define HENBUS_CRC_LENGTH				(1)
#else
#define HENBUS_CRC_LENGTH				(2)
#endif

/* Sekcja deklaracji ---------------------------------------------------------*/

// --->Funkcje

// Inicjalizacja kontrolera HENBUS
CommController_t HENBUSCtrl_Init(const CommProtocolFrame_t* wdTestFrame,
                                 const CommProtocolFrame_t* wdAnswerFrame,
                                 ESPName_t serialPortName,
                                 void (*FrameCallback)(CommProtocolFrame_t*),
                                 uint16_t taskInterval);

#endif								/* HENBUS_CONTROLLER_H */

/******************* (C) COPYRIGHT 2013 HENIUS *************** KONIEC PLIKU ***/