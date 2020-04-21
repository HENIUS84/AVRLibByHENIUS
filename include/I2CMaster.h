/**
 *******************************************************************************
 * @file     I2Cmaster.h                                                      
 * @author   HENIUS (Paweł Witak)                                              
 * @version  1.1.3
 * @date     03/04/2014                                                        
 * @brief    Zestaw funkcji związanych z obsługą magistrali I2C                
 *           (plik nagłówkowy) - tryb MASTER                                  
 *******************************************************************************
 *
 * <h2><center>COPYRIGHT 2014 HENIUS</center></h2>
 */

#ifndef  I2C_MASTER_H
#define  I2C_MASTER_H

/* Sekcja include ------------------------------------------------------------*/

// Pliki systemowe
#include <stdbool.h>
#include <stdint.h>
#include <math.h>

// Pliki użytkownika

/* Sekcja stałych, makr i definicji ------------------------------------------*/

// --->Stałe

#define I2C_MAX_OF_TWPS		(2)		/*!< Maksymalna wartość rejestru TWPS */
/*! Maksymalny błąd częstotliwości magistrali (w Hz) */
#define I2C_SCK_MAX_ERROR	(10000)
/*! Wielkość określająca czas oczekiwania na zwolnienie transceiver'a */
#define I2C_TRANSCEIVER_BUSY_TIMEOUT	(10000)

// Ustawienia magistrali

/*! Rozmiar bufora nadawczego i odbiorczego */
#define I2C_BUFFER_SIZE    	(100)
/*! Pozycja części adresowej w bajcie SLA+R/W */	
#define I2C_ADDR_BITS		(1)		
/*! Pozycja bitu R/!W w bajcie adresowym */
#define I2C_READ_BIT  		(_BV(I2C_ADDR_BITS - 1))
//#define I2C_DEBUG_ENABLED			/*!< Flaga aktywująca tryb DEBUG dla I2C */


// --->Makra

/*! Obliczanie częstotliwości zegara transmisji */
#define I2C_GET_FREQUENCY(CPU_Freq, twbr_reg, twps_reg) \
	round((double)CPU_Freq / (16 + twbr_reg * (2 << (2 * twps_reg))))
/*! Obliczanie wartości rejestru TWBR */
#define I2C_GET_TWBR(CPU_Freq, I2C_Freq, twps_reg) \
	round(((double)CPU_Freq - 16 * I2C_Freq)/ ((2 << (2 * twps_reg)) * I2C_Freq))

/*! Status magistrali I2C */
#define I2C_STATUS			(TWSR & 0xF8)

// --->Typy

/**
 * @brief Stan interfejsu I2C
 */
typedef enum
{
	// Główne kody statusu TWI Master
	
	I2C_START         = 0x08,  		/*!< Bit START'u wysłany */
	/*! Transmitowano powtórzony bit START'u */
	I2C_REP_START     = 0x10,   	
	I2C_ARB_LOST      = 0x38,   	/*!< Problem z arbitrażem */

	// Kody statusu transmitera TWI Master
	I2C_MTX_ADR_ACK   = 0x18,  		/*!< SLA+W został wyslany i ACK odebrany */
	I2C_MTX_ADR_NACK  = 0x20,  		/*!< SLA+W zostal wysłany i NACK odebrany */
	/*! Bajt danych został wysłany i ACK odebrany */
	I2C_MTX_DATA_ACK  = 0x28,  	
	/*! Bajt danych został wysłany i NACK odebrany */	
	I2C_MTX_DATA_NACK = 0x30,       

	// Kody statusu odbiornika TWI Master
	
	I2C_MRX_ADR_ACK   = 0x40,  		/*!< SLA+R został wysłany i ACK odebrany */
	I2C_MRX_ADR_NACK  = 0x48,  		/*!< SLA+R został wysłany i NACK odebrany */
	/*! Bajt danych został odebrany i ACK wysłany */
	I2C_MRX_DATA_ACK  = 0x50,  		
	/*! Bajt danych został odebrany i NACK wysłany */
	I2C_MRX_DATA_NACK = 0x58,  		
	// Pozostałe kody statusu TWI
	I2C_NO_STATE      = 0xF8,  		/*!< Brak istotnego stanu */
	/*! Błąd magistrali ze względu na niewłaściwe warunki START or STOP */
	I2C_BUS_ERROR     = 0x00,  		
}EI2CState_t;

/**
 * @brief Status połączenia I2C
 */
typedef enum
{
	I2CMS_OK,						/*!< Wszystko OK */
	I2CMS_TRANSCEIVER_NOT_READY		/*!< Brak gotowości nadajnika */
}EI2CMasterStatus_t;

/**
 * @brief Częstotliwość zegara I2C
 */
typedef enum
{
	/*! Częstotliwość 400 kHz */
	I2CC_400K = 400000,	
	/*! Częstotliwość 200 kHz */
	I2CC_200K = 200000,	
	/*! Częstotliwość 100 kHz */
	I2CC_100K = 100000,	
	/*! Częstotliwość 50 kHz */
	I2CC_50K  = 50000		
}EI2Cclock_t;

/**
 * @brief Rejestr statusowy interfejsu
 */
typedef union
{
   uint8_t All;

   struct
	{
		/*! Stan ostatniej transmisji */
		uint8_t IsLastSendingOk   : 1;
		/*! Stan ostatniego odbioru */
		uint8_t IsLastReceivingOk : 1;
		uint8_t UnusedBits  : 6;	/*!< Nieużywane bity */
	 };
}I2CStatusReg_t;

/**
 * @brief Konfiguracja magistrali I2C
 */
typedef struct
{
	EI2Cclock_t ClockRate;			/*!< Częstotliwość zegara w Hz */		
	uint32_t CpuFrequency;			/*!< Częstotliwość CPU w MHz */
}I2CMaster_t;

/* Sekcja deklaracji ---------------------------------------------------------*/

// Inicjalizacja modułu I2C
void I2CMaster_Init(I2CMaster_t* i2cConfig);
// Deinicjalizacja modułu I2C
void I2CMaster_Deinit(void);
// Sprawdzanie zajętości nadajnika/odbiornika
bool I2CMaster_IsTransceiverBusy();
// Wysyłanie danych do Slave'a (operacja zapis/odczyt), pierwszy bajt
// to bajt adresowy
bool I2CMaster_SendData(uint8_t *message, uint8_t messageSize);
// Odczyt danych z bufora interfejsu
bool I2CMaster_ReadData(uint8_t *message, uint8_t messageSize);
/*! Odczyt statusu transmisji */
I2CStatusReg_t I2CMaster_GetStatus();

#endif										/* I2C_MASTER_H */

/******************* (C) COPYRIGHT 2014 HENIUS *************** KONIEC PLIKU ***/
