/**
 *******************************************************************************
 * @file     I2Cslave.c
 * @author   HENIUS (Paweł Witak)
 * @version  1.1.1
 * @date     04/03/2011
 * @brief    Zestaw funkcji związanych z obsługą magistrali I2C - tryb Slave
 *******************************************************************************
 *
 * <h2><center>COPYRIGHT 2011 HENIUS</center></h2>
 */

/* Sekcja include ------------------------------------------------------------*/

// --->Pliki systemowe

#include <stdio.h>
#include <stdbool.h>
#include <avr/io.h>
#include <stdint.h>
#include <avr/iom32.h>
#include <avr/interrupt.h>

// --->Pliki użytkownika

#include "I2CSlave.h"

/* Sekcja zmiennych ----------------------------------------------------------*/

/*! Wskaźnik do konfiguracji magistrali I2C */	
I2CSlave_t* I2CslaveCfg;			
uint8_t I2CMsgSize;                 /*!< Liczba wysyłanych bajtów I2C */
uint8_t I2Cbuff[I2C_BUFFER_SIZE];   /*!< Bufor nadawczy I2C */

/* Sekcja funkcji ------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/**
 * @brief    Inicjalizacja modułu I2C - slave
 * @param    i2cConfig : wskaźnik do struktury z konfiguracją interfejsu
 * @retval   Brak
 */
void I2CSlave_Init(I2CSlave_t* i2cConfig)
{
	I2CslaveCfg = i2cConfig;

	// Ustalanie adresu urządzenia
	TWAR = i2cConfig->Address;

	TWDR = 0xFF;                    // Domyślna zawartość = SDA zwolnione
	TWCR = (1 << TWEN)  |           // Aktywacja interfejsu I2C
			                        // i zwolnienie pinów I2C
		   (0 << TWIE)  |           // Deaktywacja przerwania
		   (0 << TWINT) |
		   (0 << TWEA)  |           // Jeszcze brak żądania wystawiania ACK
		   (0 << TWSTA) |
		   (0 << TWSTO) |
		   (0 << TWWC);
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Sprawdzanie zajętości nadajnika/odbiornika
 * @param    Brak
 * @retval   Flaga oznaczająca zajętość
 */
bool I2CSlave_TransceiverBusy()
{
	bool result;

	result = (TWCR & _BV(TWIE)) ? true : false;

	return result;
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Czekanie na zakończenie zadań przez nadajnik
 * @param    Brak
 * @retval   Brak
 */
void I2Cslave_WaitForTransceiver()
{
	while (I2CSlave_TransceiverBusy());
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Deinicjalizacja modułu I2C
 * @param    Brak
 * @retval   Brak
 */
void I2Cslave_Deinit()
{
	// Wyłączenie interfejsu I2C
	TWCR = (0 << TWEN)  |           // Deaktywacja interfejsu I2C
		   (0 << TWIE)  |           // Deaktywacja przerwania
		   (0 << TWINT) |
		   (0 << TWEA)  |
		   (0 << TWSTA) |
		   (0 << TWSTO) |
		   (0 << TWWC);
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Wysyłanie danych do Master'a (operacja zapis/odczyt)
 * @param    message - wksaźnik do bufora wiadomości
 * @param    messageSize - liczba bajtów wiadomości do wysłania
 * @retval   Brak
 */
void I2CSlave_SendData(uint8_t* message, uint8_t messageSize)
{
	uint8_t index;

	I2Cslave_WaitForTransceiver();	// Czekanie na zakończenie pracy I2C

	// Przygotowanie do wysłania danych
	I2CMsgSize = messageSize;

	// Zabezpieczenie przed zbyt długimi komunikatami
	if (messageSize > I2C_BUFFER_SIZE)
	{
		messageSize = I2C_BUFFER_SIZE;
	}

	// Zapamiętanie komunikatu
	for (index = 0; index < messageSize; index++)
	{
		I2Cbuff[index] = message[index];
	}

	// Rozpoczęcie wysłania
	TWCR = (1 << TWEN)  |   		// Uruchomienie interfejsu TWI
		   (1 << TWIE)  |   		// Aktywacja przerwania
		   (1 << TWINT) |   		// Kasowanie flagi przerwania
		   (0 << TWSTO) |
		   (1 << TWEA)  |			// Potwierdzanie transmisji
		   (0 << TWSTA) |
		   (0 << TWWC);
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Start transceiver'a
 * @param    Brak
 * @retval   Brak
 */
void I2CSlave_StartTransceiver()
 {
	 I2Cslave_WaitForTransceiver();	// Czekanie na zakończenie pracy I2C
	 TWCR = (1 << TWEN)  |         	// Aktywacja interfejsu
		    (1 << TWIE)  |			// Kasowanie flagi przerwania
		    (1 << TWINT) |         	// Aktywacja przerwania
		    (1 << TWEA)  |			// Włączenie potwierdzenia ACK
		    (0 << TWSTA) |
		    (0 << TWSTO) |
		    (0 << TWWC);
 }

/*----------------------------------------------------------------------------*/
/**
 * @brief    Odczyt danych z bufora interfejsu
 * @param    message - wksaźnik do bufora wiadomości na odebrane dane
 * @param    messageSize - liczba bajtów wiadomości do odebrania
 * @retval   Brak
 */
void I2CSlave_ReadData(uint8_t *message, uint8_t messageSize)
{
	uint8_t index;
	
	I2Cslave_WaitForTransceiver();	// Czekanie na zakończenie pracy I2C

	// Odbieranie danych jeśli poprzednia transmisja zakończyła
	// się powodzeniem.
	if (I2CslaveCfg->Status.LastTransOK)
	{	
		if (messageSize > I2C_BUFFER_SIZE)
		{
			messageSize = I2C_BUFFER_SIZE;
		}

		// Zapis danych do wybranego bufora
		for (index = 0; index < messageSize; index++)
		{
			message[index] = I2Cbuff[index];
		}

		// Dane zostały odebrane
		I2CslaveCfg->Status.RxDataInBuf = false;
	}
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Funkcja obsługi przerwania I2C
 * @param    Brak
 * @retval   Brak
 */
ISR(TWI_vect)
{
	static unsigned char buffIndex;

	// Sprawdzanie statusu I2C
	I2CslaveCfg->State = I2C_STATUS;

	// Sprawdzanie stanu połączenia
	if (I2CslaveCfg->State == I2C_BUS_ERROR)
	{
		// Nadajnik niegotowy
		I2CslaveCfg->ConnStatus = I2CMS_TRANSCEIVER_NOT_READY;
	}
	else
	{
		// Połączenie I2C sprawne
		I2CslaveCfg->ConnStatus = I2CMS_OK;
	}

	// Maszyna stanów interfejsu
	switch(I2CslaveCfg->State)
	{
		// ---> Nadajnik
		
		// --->Własny SLA+R został odebrany, ACK zwrócony
		case I2C_STX_ADR_ACK:
			buffIndex   = 0;		// Reset indeksu bufora danych

		// --->Dana z TWDR została wysłana, ACK został odebrany
		case I2C_STX_DATA_ACK:
			// Wysłanie kolejnego bajtu danych
			TWDR = I2Cbuff[buffIndex++];
			TWCR = (1 << TWEN)  |	// Aktywacja interfejsu I2C
				   (1 << TWIE)  |	// Aktywacja przerwania
				   (1 << TWINT) |	// Kasowanie flagi przerwania
				   (1 << TWEA)  |
				   (0 << TWSTA) |
				   (0 << TWSTO) |
				   (0 << TWWC);

			break;

		// --->Dana z TWDR została wysłana, NACK został odebrany.
		// Np. może to być koniec transmisji.
		case I2C_STX_DATA_NACK:
			// Czy wysłano wszystkie dane?
			if (buffIndex == I2CMsgSize)
			{
				 // Wysłano wszystkie dane
				I2CslaveCfg->Status.LastTransOK = true;
			}

			// Transceiver I2C w trybie pasywnym
			TWCR = (1 << TWEN)  |	// Aktywacja interfejsu
			       (0 << TWIE)  |	// Wyłączanie przerwań
				   (0 << TWINT) |
				   (0 << TWEA)  |
				   (0 << TWSTA) |
				   (0 << TWSTO) | 	// Brak potwierdzania ACK
				   (0 << TWWC);

			break;

		// ---> Odbiornik
		
		// --->Adres głównego wywołania odebrany, ACK zwrócone
		case I2C_SRX_GEN_ACK:
			I2CslaveCfg->Status.GenAddressCall = true;

		// --->Własny SLA+W odebrany i ACK zwrócone
		case I2C_SRX_ADR_ACK:
			// Nie ma potrzeby czyszczenia flagi Status.GenAddressCall, ponieważ
			// to domyślny stan.
			I2CslaveCfg->Status.RxDataInBuf = true;
			buffIndex = 0;			// Reset wskaźnika bufora
			TWCR = (1 << TWEN)  |	// Aktywacja interfejsu
				   (1 << TWIE)  | 	// Aktywacja przerwania
				   (1 << TWINT) | 	// Kasowanie flagi przerwania
				   (1 << TWEA)  | 	// Generowanie sygnału ACK
				   (0 << TWSTA) |
				   (0 << TWSTO) |
				   (0 << TWWC);

			break;

	   // --->Poprzednio zaadresowano SLA+W, dane odebrano, ACK zwrócono
	   case I2C_SRX_ADR_DATA_ACK:

	   // --->Poprzednio zaadresowano głównym adresem wywołania, dane odebrano,
	   // ACK zwrócono
	   case I2C_SRX_GEN_DATA_ACK:
	   		// Zapis danych do bufora
	   		I2Cbuff[buffIndex++] = TWDR;

	   		// Ustawianie flagi od poprawnej transmisji
	   		I2CslaveCfg->Status.LastTransOK = true;

	   		TWCR = (1 << TWEN)  |		// Aktywacja interfejsu
				   (1 << TWIE)  | 		// Aktywacja przerwania
				   (1 << TWINT) | 		// Kasowanie flagi przerwania
				   (1 << TWEA)  | 		// Generowanie sygnału ACK
				   (0 << TWSTA) |
				   (0 << TWSTO) |
				   (0 << TWWC);

			  break;

	   // --->Warunek STOP lub powtórzony START odebrano przy zaadresowanym Slave'ie
	   case I2C_SRX_STOP_RESTART:
	   		// Transceiver w trybie pasywnym
	   		TWCR = (1 << TWEN)  |		// Aktywacja interfejsu i zwolnienie pinów
				   (0 << TWIE)  | 		// Dezaktywacja przerwania
				   (0 << TWINT) |
				   (0 << TWEA)  | 		// Bez potwierdzenia żądań
				   (0 << TWSTA) |
				   (0 << TWSTO) |
				   (0 << TWWC);

			break;

		// --->Poprzednio zaadresowano SLA+W, dane odebrano, NACK zwrócono
		case I2C_SRX_ADR_DATA_NACK:

		// --->Poprzednio zaadresowano głównym adresem wywołania, dane odebrano,
		// NACK zwrócono
		case I2C_SRX_GEN_DATA_NACK:

		// --->Ostatni bajt został wysłany z TWDR (TWEA = 0), ACK odebrano
		case I2C_STX_DATA_ACK_LAST_BYTE:

		// ---> Błędy
		
		// --->Błąd magistrali ze względu na niewłaściwe warunki START/STOP
		case I2C_BUS_ERROR:

		// --->Inny stan
		default:
			// Zakończenie transmisji (wygenerowanie STOP'u), master może wówczas
			// uzyskać ewentualną informację o błędzie magistrali.
			TWCR = (1 << TWEN)  |	// Aktywacja interfejsu i zwolnienie pinów
			       (1 << TWIE)  | 	// Dezaktywacja przerwania
				   (1 << TWINT) |
				   (1 << TWEA)  | 	// Bez potwierdzenia żądań
				   (0 << TWSTA) |
				   (1 << TWSTO) |	// Generowanie bitu STOP'u
				   (0 << TWWC);
	}
}

/******************* (C) COPYRIGHT 2010 HENIUS *************** KONIEC PLIKU ***/
