/**
 *******************************************************************************
 * @file     I2Cslave.h                                                       
 * @author   HENIUS (Paweł Witak)                                              
 * @version  1.01.001                                                          
 * @date     04/03/2011                                                        
 * @brief    Zestaw funkcji związanych z obsługą magistrali I2C                
 *           (plik nagłówkowy) - tryb SLAVE                                    
 *******************************************************************************
 *
 * <h2><center>COPYRIGHT 2011 HENIUS</center></h2>
 */

#ifndef  I2C_SLAVE_H
#define  I2C_SLAVE_H

/* Sekcja include ------------------------------------------------------------*/

// Pliki systemowe
#include <stdbool.h>

/* Sekcja stałych, makr i definicji ------------------------------------------*/

// --->Stałe
// Ustawienia magistrali
#define I2C_BUFFER_SIZE    	(100)	/*!< Rozmiar bufora nadawczego */
#define I2C_ADDR_BITS		(1)		/*!< Pozycja częsci adresowej w bajcie
                                         SLA+R/W */

// --->Makra
/*! Reset timera timeout'u */
#define I2C_TIMEOUT_RESET()	(I2Ctimeout = I2C_TIMEOUT)

/* ! Status magistrali I2C */
#define I2C_STATUS			(TWSR & 0xF8)

// --->Typy

/**
 * @brief Stan interfejsu I2C
 */
typedef enum
{
	// Kody statusu transmitera I2C Slave
	I2C_STX_ADR_ACK            = 0xA8,	/*!< Odebrano własny SLA+R,
	                                         ACK zwrócono */
	I2C_STX_ADR_ACK_M_ARB_LOST = 0xB0,  /*!< Arbitraż stracono w SLA+R/W
	                                         (Master), odebrano SLA+R,
	                                         ACK zwrócono */
	I2C_STX_DATA_ACK           = 0xB8,  /*!< Wysłano bajt z TWDR, ACK odebrano */
	I2C_STX_DATA_NACK          = 0xC0,  /*!< Wysłano bajt z TWDR,
	                                         NACK odebrano */
	I2C_STX_DATA_ACK_LAST_BYTE = 0xC8,  /*!< Wysłano ostatni bajt z TWDR
	                                         (TWEA = 0); ACK odebrany */

	// Kody statusu odbiornika I2C Slave
	I2C_SRX_ADR_ACK            = 0x60,  /*!< Odebrano własny SLA+W,
	                                         ACK zwrócono */
	I2C_SRX_ADR_ACK_M_ARB_LOST = 0x68,  /*!< Arbitraż stracono w SLA+R/W
	                                         (Master), odebrano SLA+W,
	                                         ACK zwrócono */
	I2C_SRX_GEN_ACK            = 0x70,  /*!< Odebrano adres głównego
	                                         wywołania, ACK zwrócono */
	I2C_SRX_GEN_ACK_M_ARB_LOST = 0x78,  /*!< Arbitraż stracono w SLA+R/W
	                                         (Master), odebrano adres głównego
	                                         wywołania, ACK zwrócono */
	I2C_SRX_ADR_DATA_ACK       = 0x80,  /*!< Poprzednio zaadresowano SLA+W,
	                                         dane odebrano, ACK zwrócono */
	I2C_SRX_ADR_DATA_NACK      = 0x88,  /*!< Poprzednio zaadresowano SLA+W,
	                                         dane odebrano, NACK zwrócono */
	I2C_SRX_GEN_DATA_ACK       = 0x90,  /*!< Poprzednio zaadresowano głównym
	                                         wywołaniem, dane odebrano,
	                                         ACK zwrócono */
	I2C_SRX_GEN_DATA_NACK      = 0x98,  /*!< Poprzednio zaadresowano głównym
	                                         wywołaniem, dane odebrano,
	                                         NACK zwrócono */
	I2C_SRX_STOP_RESTART       = 0xA0,  /*!< Warunek STOP lub powtórzony warunek
	                                         START został odebrany przy stale
	                                         zaadresowanym SLAVE */

	// Pozostałe kody statusu I2C
	I2C_NO_STATE               = 0xF8,  /*!< Brak istotnego stanu */
	I2C_BUS_ERROR              = 0x00,  /*!< Błąd magistrali ze względu na
	                                         niewłaściwe warunki START
	                                         lub STOP */

}EI2CState_t;

/**
 * @brief Stan połączenie po I2C
 */
typedef enum
{
	I2CMS_OK,						/*!< Wszystko OK */
	I2CMS_TRANSCEIVER_NOT_READY		/*!< Brak gotowości nadajnika */
}EI2CSlaveStatus_t;

/**
 * @brief Rejestr statusowy interfejsu
 */
typedef union
{
   uint8_t All;

   struct
	{
       uint8_t LastTransOK    : 1;	/*!< Stan ostatniej transmisji */
	   uint8_t RxDataInBuf    : 1;	/*!< Nowe dane odebrane */
	   uint8_t GenAddressCall : 1;  /*!< Podano adres głównego wywołania */
	   uint8_t UnusedBits     : 5;	/*!< Nieużywane bity */
	 };
}I2CStatusReg_t;

/**
 * @brief Konfiguracja magistrali I2C
 */
typedef struct
{
	uint8_t Address;				/*!< Adres urządzenia SLAVE */
	EI2CSlaveStatus_t ConnStatus;	/*!< Status magistrali I2C Master */
	I2CStatusReg_t Status;          /*!< Status operacji interfejsu */
	EI2CState_t State;				/*!< Aktualny stan interfejsu */
}I2CSlave_t;

/* Sekcja deklaracji ---------------------------------------------------------*/

// Inicjalizacja modułu I2C
void I2CSlave_Init(I2CSlave_t* i2cConfig);
// Start transceiver'a
void I2CSlave_StartTransceiver();
// Sprawdzanie zajętości nadajnika/odbiornika
bool I2CSlave_TransceiverBusy();
// Wysyłanie danych do Master'a (operacja zapis/odczyt)
void I2CSlave_SendData(uint8_t* message, uint8_t messageSize);
// Odczyt danych z bufora interfejsu
void I2CSlave_ReadData(uint8_t* message, uint8_t messageSize);
// Odczyt danych z bezpośrednio z bufora interfejsu

#endif										/* I2C_SLAVE_H */

/******************* (C) COPYRIGHT 2010 HENIUS *************** KONIEC PLIKU ***/
