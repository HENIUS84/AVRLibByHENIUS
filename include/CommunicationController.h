/**
 *******************************************************************************
 * @file     CommunicationController.h
 * @author   HENIUS (Paweł Witak)
 * @version  1.1.2
 * @date     09-11-2013
 * @brief    Obsługa protokołów komunikacyjnych (plik nagłówkowy)
 *******************************************************************************
 *
 * <h2><center>COPYRIGHT 2013 HENIUS</center></h2>
 */

#ifndef  COMMUNICATION_CONTROLLER_H
#define  COMMUNICATION_CONTROLLER_H

/* Sekcja include ------------------------------------------------------------*/

// --->Pliki systemowe

#include <stdbool.h>
#include <stdint.h>

/* Sekcja deklaracji ---------------------------------------------------------*/

// --->Stałe

#define COMM_BINARY_MODE	/*! Tryb binarny komunikacji */

// --->Typy

/**
 * @brief Budowa ramki protokołu
 */
typedef struct  
{
	uint8_t Address;		/*!< Adres urządzenia */
#ifdef COMM_BINARY_MODE
	uint8_t CommandID;		/*!< ID komendy */
#else
	uint8_t* CommandName;	/*!< Nazwa komendy */	
#endif
	uint8_t DataSize;		/*!< Liczba bajtów danych (po dekodowaniu) */	
	uint8_t* Data;			/*!< Bufor danych */
}CommProtocolFrame_t;

/**
 * @brief Struktura opisująca kontroler komunikacyjny
 */
typedef struct
{
	/*! Wskaźnik do funkcji wysyłającej ramkę protokołu */
	void(*SendFrame)(CommProtocolFrame_t* frame);
	/*! Wskaźnik do funkcji obsługi kontrolera */
	bool(*Handler)();	
}CommController_t;

#endif								/* COMMUNICATION_CONTROLLER_H */

/******************* (C) COPYRIGHT 2013 HENIUS *************** KONIEC PLIKU ***/