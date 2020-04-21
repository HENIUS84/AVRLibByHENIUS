/**
 *******************************************************************************
 * @file     I2CMaster.c
 * @author   HENIUS (Paweł Witak)
 * @version  1.1.3
 * @date     03/04/2014
 * @brief    Zestaw funkcji związanych z obsługą magistrali I2C - tryb MASTER
 *           (na podstawie dokumentacji Atmel'a AVR315
 *******************************************************************************
 *
 * <h2><center>COPYRIGHT 2014 HENIUS</center></h2>
 */

/* Sekcja include ------------------------------------------------------------*/

// --->Pliki systemowe

#include <stdio.h>
#include <avr/pgmspace.h>
#include <avr/io.h>
#include <stdint.h>
#include <stdlib.h>
#include <avr/interrupt.h>

// --->Pliki użytkownika

#include "I2CMaster.h"
#include "Debug.h"

/* Sekcja zmiennych ----------------------------------------------------------*/

/*! Wskaźnik do konfiguracji magistrali I2C */
I2CMaster_t *I2CMasterCfg;			
uint8_t I2CMsgSize;                 /*!< Liczba wysyłanych bajtów I2C */
uint8_t I2Cbuff[I2C_BUFFER_SIZE];   /*!< Bufor nadawczy I2C */
/*! Łańcuch formatujący dla błędu I2C */
const uint8_t I2CErrorFormatter[] = "I2C Error %s\r\n";
I2CStatusReg_t Status;				/*!< Status operacji interfejsu */
EI2CState_t State;					/*!< Aktualny stan obsługi interfejsu */

/* Sekcja funkcji ------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/**
 * @brief    Inicjalizacja modułu I2C - master
 * @param    *i2cConfig : wskaźnik do struktury z konfiguracją interfejsu
 * @retval   Brak
 */
void I2CMaster_Init(I2CMaster_t *i2cConfig)
{	
	int8_t twps = I2C_MAX_OF_TWPS;	// Aktualna wartość rejestru TWPS
	
	I2CMasterCfg = i2cConfig;		// Zapamiętanie konfiguracji interfejsu
	State = I2C_NO_STATE;
	Status.All = 0;
				
	// Obliczanie nastaw zegara
	while(twps >= 0)
	{
		TWBR = I2C_GET_TWBR(i2cConfig->CpuFrequency,
		                    i2cConfig->ClockRate,
							twps);
				
		// Maksymalna odchyłka w różnicy częstotliwości nie powinna przekroczyć
		// 10 kHz			
		if (abs(i2cConfig->ClockRate -
		        I2C_GET_FREQUENCY(i2cConfig->CpuFrequency,
				                  TWBR,
								  twps)) <= I2C_SCK_MAX_ERROR)
		{
			break;
		}
		
		twps--;
	}
	
	// Ustawianie prescalera
	TWSR |= (twps << TWPS0);
	
	Status.IsLastReceivingOk = Status.IsLastSendingOk = false;	
	TWCR = (1 << TWEN)  |   		// Uruchomienie interfejsu TWI
		   (0 << TWIE)  |   		// Dezaktywacja przerwania
		   (0 << TWINT) |
		   (0 << TWSTO) |
		   (0 << TWEA)  |
		   (0 << TWSTA) |
		   (0 << TWWC);
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Sprawdzanie zajętości nadajnika/odbiornika
 * @param    Brak
 * @retval   Brak
 */
bool I2CMaster_IsTransceiverBusy(void)
{
	bool result;

	return (TWCR & _BV(TWIE)) ? true : false;
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Czekanie na zakończenie zadań przez nadajnik
 * @param    Brak
 * @retval   Status operacji (true - sukces)
 */
bool I2CMaster_WaitForTransceiver(void)
{
	uint16_t timeoutTimer = I2C_TRANSCEIVER_BUSY_TIMEOUT;
	
	while (I2CMaster_IsTransceiverBusy() && --timeoutTimer);
	
	// Próba odblokowania trensceiver'a
	if (!timeoutTimer)
	{
		I2CMaster_Deinit();
	}
	
	return timeoutTimer ? true : false;
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Deinicjalizacja modułu I2C
 * @param    Brak
 * @retval   Brak
 */
void I2CMaster_Deinit(void)
{
	// Wyłączenie systemu I2C
	TWCR &= ~_BV(TWEN) &
	        ~_BV(TWIE);					
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Wysyłanie danych do Slave'a (operacja zapis/odczyt), pierwszy bajt
 *           jest bajtem adresowym.
 * @param    message - wskaźnik do bufora wiadomości
 * @param    messageSize - liczba bajtów wiadomości do wysłania
 * @retval   Status operacji (true - sukces)
 */
bool I2CMaster_SendData(uint8_t* message, uint8_t messageSize)
{
	// Indeks pomocniczy
	uint8_t index;
	// Czekanie na zakończenie pracy I2C i zwracanie rezultatu
	bool result = I2CMaster_WaitForTransceiver();	

	// Przygotowanie do wysłania danych
	I2CMsgSize = messageSize + 1;	// Zapamiętanie rozmiaru wiadomości
									// (plus bajt SLA+W/R)
	I2Cbuff[0] = message[0];		// Zapamiętanie adresu SLAVE'a i danych R/W

	// Zabezpieczenie przed zbyt długimi komunikatami
	if (messageSize > I2C_BUFFER_SIZE)
	{
		messageSize = I2C_BUFFER_SIZE;
	}

	// Zapamiętanie komunikatu jeśli to jest operacja zapisu
	if (!(message[0] & I2C_READ_BIT))
	{
		for (index = 1; index < messageSize; index++)
		{
			I2Cbuff[index] = message[index];
		}
	}

	// Rozpoczęcie wysłania
	Status.All = 0;
	TWCR = (1 << TWEN)  |   			// Uruchomienie interfejsu TWI
		   (1 << TWIE)  |   			// Aktywacja przerwania
		   (1 << TWINT) |   			// Kasowanie flagi przerwania
		   (0 << TWSTO) |
		   (0 << TWEA)  |
		   (1 << TWSTA) |   			// Wysłanie bitu START'u
		   (0 << TWWC);
		   
	return result;
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Odczyt danych z bufora interfejsu
 * @param    message - wskaźnik do bufora wiadomości na odebrane dane
 * @param    messageSize - liczba bajtów wiadomości do odebrania
 * @retval   Status operacji (true - sukces)
 */
bool I2CMaster_ReadData(uint8_t* message, uint8_t messageSize)
{
	// Indeks pomocniczy
	uint8_t index;
	// Czekanie na zakończenie pracy I2C i zwracanie rezultatu
	bool result = I2CMaster_WaitForTransceiver();

	// Zabezpieczenie przed zbyt długimi komunikatami
	if (messageSize > (I2C_BUFFER_SIZE - 1))
	{
		messageSize = I2C_BUFFER_SIZE - 1;
	}

	// Odbieranie danych jeśli poprzednia transmisja zakończyła
	// się powodzeniem.
	if (Status.IsLastReceivingOk)
	{
		for (index = 0; index < messageSize; index++)
		{
			message[index] = I2Cbuff[index + 1];
		}
	}
	
	return result;
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Logowanie statusu I2C
 * @param    state - status komunikacji I2C
 * @retval   Brak
 */
void LogI2Cstatus(EI2CState_t state)
{	
#ifdef I2C_DEBUG_ENABLED
	uint8_t info[] = "I2C Info: %s\r\n";
	uint8_t error[] = "I2C Error: %s\r\n";
	
	switch(state)
	{
		// Normalna praca (bez błędów)

		// Bit START'u został wysłany
		case I2C_START:
			DebugWrite(info, "I2C_START");
			
			break;

		// Powtórzony bit START'u wysłany
		case I2C_REP_START:
			DebugWrite(info, "I2C_REP_START");
			
			break;

		// SLA+W został wysłany i ACK odebrany
		case I2C_MTX_ADR_ACK:
			DebugWrite(info, "I2C_MTX_ADR_ACK");
			
			break;

		// Bajt danych został wysłany i ACK odebrany
		case I2C_MTX_DATA_ACK:		
			DebugWrite(info, "I2C_MTX_DATA_ACK");

		// Bajt danych został odebrany i ACK wysłany
		case I2C_MRX_DATA_ACK:
			DebugWrite(info, "I2C_MRX_DATA_ACK");
			
			break;

		// SLA+R został wysłany i ACK odebrany
		case I2C_MRX_ADR_ACK:
			DebugWrite(info, "I2C_MRX_ADR_ACK");
			
			break;

		// Bajt danych został odebrany i NACK wysłany
		case I2C_MRX_DATA_NACK:
			DebugWrite(info, "I2C_MRX_DATA_NACK");
			
			break;

		// Stracono arbitraż (błąd magistrali)
		case I2C_ARB_LOST:
			DebugWrite(info, "I2C_ARB_LOST");
			
			break;

		// Obsługa błędów

		// SLA+W został wysłany i NACK odebrany
		case I2C_MTX_ADR_NACK:
			DebugWrite(error, "I2C_MTX_ADR_NACK");
			
			break;

		// SLA+R został wysłany i NACK odebrany
		case I2C_MRX_ADR_NACK:
			DebugWrite(error, "I2C_MRX_ADR_NACK");
			
			break;

		// Bajt danych został wysłany i NACK odebrany
		case I2C_MTX_DATA_NACK:
			DebugWrite(error, "I2C_MTX_DATA_NACK");
			
			break;

		// Błąd magistrali ze względu na niewłaściwe warunki START/STOP
		case I2C_BUS_ERROR:
			DebugWrite(error, "I2C_BUS_ERROR");
			
			break;
		
		// Każdy dowolny inny stan
		default:		
			break;
	}
#endif								/* I2C_DEBUG_ENABLED */		
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Odczyt statusu transmisji I2C
 * @param    Brak
 * @retval   Status transmisji
 */
I2CStatusReg_t I2CMaster_GetStatus()
{
	return Status;
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Funkcja obsługi przerwania I2C
 * @param    Brak
 * @retval   Brak
 */
ISR(TWI_vect)
{
	static uint8_t buffIndex;

	// Sprawdzanie statusu I2C
	State = I2C_STATUS;

	// Maszyna stanów interfejsu
	switch(State)
	{
		// Normalna praca (bez błędów)

		// --->Bit START'u został wysłany
		case I2C_START:

		// --->Powtórzony bit START'u wysłany
		case I2C_REP_START:
			buffIndex = 0;

		// --->SLA+W został wysłany i ACK odebrany
		case I2C_MTX_ADR_ACK:

		// --->Bajt danych został wysłany i ACK odebrany
		case I2C_MTX_DATA_ACK:

			if (buffIndex < I2CMsgSize)
			{
				// Jeszcze jest coś do wysłania.
				Status.IsLastSendingOk = false;
				TWDR = I2Cbuff[buffIndex++];
				TWCR = (1 << TWEN)  |   // Uruchomienie interfejsu TWI
				       (1 << TWIE)  |   // Aktywacja przerwania
				       (1 << TWINT) |   // Kasowanie flagi przerwania
				       (0 << TWSTO) |
				       (0 << TWEA)  |
				       (0 << TWSTA) |
				       (0 << TWWC);
			}
			else
			{
				// Już nie ma nic do wysłania :)
				Status.IsLastSendingOk = true;
				TWCR = (1 << TWEN)  |   // Uruchomienie interfejsu TWI
				       (0 << TWIE)  |   // Dezaktywacja przerwania
				       (1 << TWINT) |   // Kasowanie flagi przerwania
				       (1 << TWSTO) |   // Wysłanie bitu stopu
				       (0 << TWEA)  |
				       (0 << TWSTA) |
				       (0 << TWWC);
			}

			break;

		// --->Bajt danych został odebrany i ACK wysłany
		case I2C_MRX_DATA_ACK:
			I2Cbuff[buffIndex++] = TWDR;

		// --->SLA+R został wysłany i ACK odebrany
		case I2C_MRX_ADR_ACK:
			Status.IsLastReceivingOk = false;
		
			if (buffIndex < (I2CMsgSize - 1))
			{
				// Jeśli to nie przedostatni bajt to wyślij ACK.
				TWCR = (1 << TWEN)  |   // Uruchomienie interfejsu TWI
					   (1 << TWIE)  |   // Aktywacja przerwania
					   (1 << TWINT) |   // Kasowanie flagi przerwania
					   (0 << TWSTO) |
					   (1 << TWEA)  |	// Wysłanie ACK po odebraniu bajtu
					   (0 << TWSTA) |
					   (0 << TWWC);
			}
			else
			{
				// To jest przedostatni bajt.
				// Po ostatnim bajcie ma być wysłany NACK
				TWCR = (1 << TWEN)  |   // Uruchomienie interfejsu TWI
					   (1 << TWIE)  |   // Aktywacja przerwania
					   (1 << TWINT) |   // Kasowanie flagi przerwania
					   (0 << TWSTO) |
					   (0 << TWEA)  |	// NACK po odebraniu ostatniego bajtu
					   (0 << TWSTA) |
					   (0 << TWWC);
			}

			break;

		// --->Bajt danych został odebrany i NACK wysłany
		case I2C_MRX_DATA_NACK:
			// Zapisanie ostatniego bajtu
			I2Cbuff[buffIndex] = TWDR; 

			// To już koniec transmisji (odbierania danych) :)
			Status.IsLastReceivingOk = true;
			TWCR = (1 << TWEN)  |   // Uruchomienie interfejsu TWI
				   (0 << TWIE)  |   // Dezaktywacja przerwania
				   (1 << TWINT) |   // Kasowanie flagi przerwania
				   (1 << TWSTO) |   // Wysłanie bitu STOP'u
				   (0 << TWEA)  |	// NACK po odebraniu ostatniego bajtu
				   (0 << TWSTA) |
				   (0 << TWWC);

			break;

		// --->Stracono arbitraż (błąd magistrali)
		case I2C_ARB_LOST:
			TWCR = (1 << TWEN)  |   // Uruchomienie interfejsu TWI
				   (1 << TWIE)  |   // Aktywacja przerwania
				   (1 << TWINT) |   // Kasowanie flagi przerwania
				   (0 << TWSTO) |
				   (0 << TWEA)  |	// NACK po odebraniu ostatniego bajtu
				   (1 << TWSTA) |   // RE(START)
				   (0 << TWWC);

			break;

		// Obsługa błędów

		// --->SLA+W został wysłany i NACK odebrany
		case I2C_MTX_ADR_NACK:

		// --->SLA+R został wysłany i NACK odebrany
		case I2C_MRX_ADR_NACK:

		// --->Bajt danych został wysłany i NACK odebrany
		case I2C_MTX_DATA_NACK:

		// --->Błąd magistrali ze względu na niewłaściwe warunki START/STOP
		case I2C_BUS_ERROR:
		
		// --->Każdy dowolny inny stan
		default:
			LogI2Cstatus(State);
			TWCR = (1 << TWEN)  |   // Uruchomienie interfejsu TWI
				   (0 << TWIE)  |   // Dezaktywacja przerwania
				   (0 << TWINT) |
				   (0 << TWSTO) |
				   (0 << TWEA)  |
				   (0 << TWSTA) |
				   (0 << TWWC);			
	}
}

/******************* (C) COPYRIGHT 2014 HENIUS *************** KONIEC PLIKU ***/
