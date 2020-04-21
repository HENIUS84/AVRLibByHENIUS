/**
 *******************************************************************************
 * @file     SerialPort.c
 * @author   HENIUS (Paweł Witak)
 * @version  1.01.003
 * @date     15/11/2013
 * @brief    Zestaw funkcji związanych z obsługa portu szeregowego z IRQ
 *******************************************************************************
 *
 * <h2><center>COPYRIGHT 2013 HENIUS</center></h2>
 */

/* Sekcja include ------------------------------------------------------------*/

// --->Pliki systemowe

#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>

// --->Pliki użytkownika

#include "SerialPort.h"

/* Sekcja zmiennych ----------------------------------------------------------*/

/*! Tablica konfiguratorów portów szeregowych */
SerialPort_t SerialPort[SP_NUMBER_OF_PORTS];
/*!< Wskaźnik do danych inicjalizujących */
SPController_t *SerialPortController;
// Funkcja wysyłająca znak dla funkcji printf
int SerialPort_Transmit(char data, FILE *stream);

/* Sekcja funkcji ------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/**
 * @brief    Inicjalizacja modułu portu szeregowego
 * @param    data : wskaźnik do danych inicjalizujących komunikację szeregową
 * @retval   Brak
 */
void SerialPort_Init(SPController_t *data)
{
	SerialPortController = data;
	if (data->IsPrintfEnabled)
	{
		fdevopen(SerialPort_Transmit, NULL);
	}
	
	// --->Inicjalizacja rejestrów
	
	// Port SPN_USART0
	SerialPort[SPN_USART0].Register.rUDR = &UDR_0;
	SerialPort[SPN_USART0].Register.rUBRRH = &UBRRH_0;
	SerialPort[SPN_USART0].Register.rUBRRL = &UBRRL_0;
	SerialPort[SPN_USART0].Register.rUCSRA = &UCSRA_0;
	SerialPort[SPN_USART0].Register.rUCSRB = &UCSRB_0;
	SerialPort[SPN_USART0].Register.rUCSRC = &UCSRC_0;
	SerialPort[SPN_USART0].Bit.bTXEN = TXEN_0;
	SerialPort[SPN_USART0].Bit.bRXEN = RXEN_0;
	SerialPort[SPN_USART0].Bit.bUCSZ0 = UCSZ0_0;
	SerialPort[SPN_USART0].Bit.bUCSZ2 = UCSZ2_0;
	SerialPort[SPN_USART0].Bit.bUCPOL = UCPOL_0;
	SerialPort[SPN_USART0].Bit.bU2X = U2X_0;
	SerialPort[SPN_USART0].Bit.bUSBS = USBS_0;
	SerialPort[SPN_USART0].Bit.bUPM0 = UPM0_0;
	SerialPort[SPN_USART0].Bit.bUMSEL = UMSEL_0;
	SerialPort[SPN_USART0].Bit.bRXCIE = RXCIE_0;
	SerialPort[SPN_USART0].Bit.bTXCIE = TXCIE_0;
	SerialPort[SPN_USART0].Bit.bRXC = RXC_0;
	SerialPort[SPN_USART0].Bit.bUDRE = UDRE_0;
	SerialPort[SPN_USART0].Bit.bUDRIE = UDRIE_0;
	
#ifdef SPN_USART1
	// Port SPN_USART1
	SerialPort[SPN_USART1].Register.rUDR = &UDR1;
	SerialPort[SPN_USART1].Register.rUBRRH = &UBRR1H;
	SerialPort[SPN_USART1].Register.rUBRRL = &UBRR1L;
	SerialPort[SPN_USART1].Register.rUCSRA = &UCSR1A;
	SerialPort[SPN_USART1].Register.rUCSRB = &UCSR1B;
	SerialPort[SPN_USART1].Register.rUCSRC = &UCSR1C;
	SerialPort[SPN_USART1].Bit.bTXEN = TXEN1;
	SerialPort[SPN_USART1].Bit.bRXEN = RXEN1;
	SerialPort[SPN_USART1].Bit.bUCSZ0 = UCSZ11;
	SerialPort[SPN_USART1].Bit.bUCSZ2 = UCSZ12;
	SerialPort[SPN_USART1].Bit.bUCPOL = UCPOL1;
	SerialPort[SPN_USART1].Bit.bU2X = U2X1;
	SerialPort[SPN_USART1].Bit.bUSBS = USBS1;
	SerialPort[SPN_USART1].Bit.bUPM0 = UPM01;
	SerialPort[SPN_USART1].Bit.Bit.UMSELbit = UMSEL1;
	SerialPort[SPN_USART1].Bit.bRXCIE = RXCIE1;
	SerialPort[SPN_USART1].Bit.bTXCIE = TXCIE1;
	SerialPort[SPN_USART1].Bit.bRXC = RXC1;
	SerialPort[SPN_USART1].Bit.bUDRE = UDRE1;
	SerialPort[SPN_USART1].Bit.bUDRIE = UDRIE1;
#endif
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Sprawdzanie statusu odczytu
 * @param    reg : rejestr portu
 * @retval : Status operacji odczytu
 */
ESPRcvStatus_t SerialPort_GetRcvStatus(uint8_t reg)
{
	ESPRcvStatus_t result;

	if (reg & _BV(FE_0))
	{
		// Błąd ramki
		result = SPRS_FRAME_ERROR;
	}
	else if (reg & _BV(DOR_0))
	{
		// Błąd nadpisania danych
		result = SPRS_DATA_OVERRUN_ERROR;
	}
	else if (reg & _BV(UPE_0))
	{
		// Błąd parzystości
		result = SPRS_PARITY_ERROR;
	}
	else
	{
		result = SPRS_OK;
	}

	return result;
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Otwarcie portu szeregowego
 * @param    serialPortName : nazwa portu
 * @param    serialPortConfig : konfiguracja portu
 * @retval   Brak
 */
void SerialPort_Open(ESPName_t serialPortName, SPDescriptor_t *serialPortConfig)
{
	uint16_t ubrr = 0;				// Rejestr prędkości
	
	if (IS_SP_EXIST(serialPortName))
	{		
		SerialPort[serialPortName].IsPortOpen = false;
				
		// Zapamiętanie konfiguracji portu
		SerialPort[serialPortName].UsartDescriptor = serialPortConfig;
	
		// --->Ustalanie liczby bitów danych
		if ((serialPortConfig->DataLength - 5) & 0x04)
		{
			*SerialPort[serialPortName].Register.rUCSRB = 
				_BV(SerialPort[serialPortName].Bit.bUCSZ0);
		}
		*SerialPort[serialPortName].Register.rUCSRC = 
			(((serialPortConfig->DataLength - 5) & 0x03) << 
			   SerialPort[serialPortName].Bit.bUCSZ0) |
		// --->Ustalanie liczby bitów stopu
			 ((serialPortConfig->StopBits - 1) << 
			   SerialPort[serialPortName].Bit.bUSBS) |
		// --->Ustalanie parzystości
			  (serialPortConfig->Parity << 
			   SerialPort[serialPortName].Bit.bUPM0) |
		// --->Ustalanie trybu pracy
			  (serialPortConfig->SyncMode << 
			   SerialPort[serialPortName].Bit.bUMSEL);

		// W zależności od trybu synchroniczności
		// mam rożne reakcje na zbocza.
		if (serialPortConfig->SyncMode == SPSM_SYNCHRONOUS)
		{
			// --->Ustalanie zbocza sygnału
			*SerialPort[serialPortName].Register.rUCSRC |= 
				(serialPortConfig->Edge << SerialPort[serialPortName].Bit.bUCPOL);
		}

		// Tryb podwójnej prędkości działa tylko w trybie asynchronicznym
		if (serialPortConfig->SpeedMode == SPSM_DOUBLE)
		{
			// Tryb synchroniczny
			*SerialPort[serialPortName].Register.rUCSRC |= 
				_BV(SerialPort[serialPortName].Bit.bUMSEL);	
			// Tryb podwójnej prędkości	
			*SerialPort[serialPortName].Register.rUCSRA |= 
				_BV(SerialPort[serialPortName].Bit.bU2X);
			ubrr = 
				SerialPortController->CpuFrequency / 8 / 
				serialPortConfig->BaudRate - 1;
		}
		else if (serialPortConfig->SpeedMode == SPSM_NORMAL)
		{
			// Tryb asynchroniczny
			*SerialPort[serialPortName].Register.rUCSRC &= 
				~_BV(SerialPort[serialPortName].Bit.bUMSEL); 	
			// Tryb normalnej prędkości
			*SerialPort[serialPortName].Register.rUCSRA &= 
				~_BV(SerialPort[serialPortName].Bit.bU2X); 	
			ubrr = 
				SerialPortController->CpuFrequency / 16 / 
				serialPortConfig->BaudRate - 1;
		}

		// Ustalanie parametrów transmisji
		// --->Ustalanie prędkości transmisji
		*SerialPort[serialPortName].Register.rUBRRH = (uint8_t) (ubrr >> 8);
		*SerialPort[serialPortName].Register.rUBRRL = (uint8_t) ubrr;

		// Aktywacja nadajnika/odbiornika
		
		// Aktywacja odbiornika
		*SerialPort[serialPortName].Register.rUCSRB |= 
			_BV(SerialPort[serialPortName].Bit.bRXEN) |
		// Aktywacja nadajnika
			_BV(SerialPort[serialPortName].Bit.bTXEN);
			
		// Aktywacja przerwań
		if (SerialPort[serialPortName].UsartDescriptor->IsIrqEnabled)
		{
			*SerialPort[serialPortName].Register.rUCSRB |=
				_BV(SerialPort[serialPortName].Bit.bRXCIE) |
				_BV(SerialPort[serialPortName].Bit.bUDRIE);
		}
			
		// Status portu
		SerialPort[serialPortName].IsPortOpen = true;
	}	
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Zamykanie portu szeregowego
 * @param    serialPortName : nazwa portu
 * @retval   Brak
 */
void SerialPort_Close(ESPName_t serialPortName)
{
	if (IS_SP_EXIST(serialPortName))
	{
		// Jeśli port był otwarty....
		if (SerialPort[serialPortName].IsPortOpen)
		{
			// zostanie zamknięty
			*SerialPort[serialPortName].Register.rUCSRB &= ~(
				// Dezaktywacja odbiornika
				_BV(SerialPort[serialPortName].Bit.bRXEN) | 
				// Dezaktywacja nadajnika	
				_BV(SerialPort[serialPortName].Bit.bTXEN)); 		
			SerialPort[serialPortName].IsPortOpen = false;
		}
	}
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Wysyłanie znaku bez przerwania
 * @param    serialPortName : nazwa portu
 * @param    character : znak do wysłania
 * @retval   Brak
 */
void SerialPort_SendChar(ESPName_t serialPortName, uint8_t character)
{
	if (SerialPort[serialPortName].IsPortOpen &&
	    IS_SP_EXIST(serialPortName) &&
		!SerialPort[serialPortName].UsartDescriptor->IsIrqEnabled)
	{
		// Czekanie na pusty bufor nadawczy
		while (!(*SerialPort[serialPortName].Register.rUCSRA &
		       _BV(SerialPort[serialPortName].Bit.bUDRE)));
		
		// Wysłanie znaku
		*SerialPort[serialPortName].Register.rUDR = character;
	}
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Wysyłanie znaku z przerwaniem
 * @param    serialPortName : nazwa portu
 * @param    character : znak do wysłania
 * @retval   Brak
 */
void SerialPort_SendChar_Irq(ESPName_t serialPortName, uint8_t character)
{
	uint8_t tmphead;				// Tymczasowa głowa
	
	if (IS_SP_EXIST(serialPortName) &&
	    SerialPort[serialPortName].UsartDescriptor->IsIrqEnabled)
	{		
		// Aktywacja przerwania
		*SerialPort[serialPortName].Register.rUCSRB |=
			_BV(SerialPort[serialPortName].Bit.bUDRIE);
		
		// Obliczanie indeksu bufora
		tmphead = (SerialPort[serialPortName].TxHead + 1 ) % SP_TX_BUFF_SIZE; 
		// Czekanie na wolne miejsce w buforze
		while (tmphead == SerialPort[serialPortName].TxTail);

		// Uzupełnianie bufora
		SerialPort[serialPortName].TxBuffer[tmphead] = character;           
		SerialPort[serialPortName].TxHead = tmphead;
	}	
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Odbieranie znaku bez przerwania
 * @param    serialPortName : nazwa portu
 * @param    timeout: czas oczekiwania na dane w ms
 * @retval   Odebrany znak (-1 to brak znaku)
 */
int16_t SerialPort_ReceiveChar(ESPName_t serialPortName, uint16_t timeout)
{
	uint8_t udr = -1;				// Odebrany bajt
	uint16_t timer = timeout;		// Timer oczekiwania na dane
	bool isDataReady = false;		// Flaga oznaczająca odebranie danych
	
	if (IS_SP_EXIST(serialPortName) &&
	    !SerialPort[serialPortName].UsartDescriptor->IsIrqEnabled)
	{
		// Czekanie na dane
		while (!(isDataReady = *SerialPort[serialPortName].Register.rUCSRA & 
			  _BV(SerialPort[serialPortName].Bit.bRXC)) && timer)
		{
			timer--;
			SerialPortController->Delay(1);
		}

		// Czy są jakieś odebrane dane?
		if (isDataReady)
		{
			SerialPort[serialPortName].UsartDescriptor->ReceiveStatus =
				SerialPort_GetRcvStatus(*SerialPort[serialPortName].Register.rUCSRA);
			
			// Odczyt danej z bufora
			udr = *SerialPort[serialPortName].Register.rUDR; 		
		}
		else
		{
			// Timeout
			if (!timer)
			{				
				SerialPort[serialPortName].UsartDescriptor->ReceiveStatus =
					SPRS_TIMEOUT_ERROR;
			}
			
			udr = -1;
		}
	}	

	return udr;
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Odbieranie znaku w przerwaniu
 * @param    serialPortName : nazwa portu
 * @param    timeout: czas oczekiwania na dane w ms
 * @retval   Odebrany znak (-1 to brak znaku)
 */
int16_t SerialPort_ReceiveChar_Irq(ESPName_t serialPortName, uint16_t timeout)
{
	uint8_t udr = -1; 				// Odebrany bajt
	uint16_t timer = timeout;		// Timer oczekiwania na dane

	if (IS_SP_EXIST(serialPortName) &&
	    SerialPort[serialPortName].UsartDescriptor->IsIrqEnabled)
	{
		// Czekanie na znak
		while (!(SerialPort[serialPortName].ReceivedDataLength =
			   SerialPort[serialPortName].RxHead -
			   SerialPort[serialPortName].RxTail) &&
			   timer)
		{
			timer--;
			SerialPortController->Delay(1);
		}
			
		// Czy zostało coś odebrane?
		if (SerialPort[serialPortName].ReceivedDataLength)
		{
			SerialPort[serialPortName].RxTail = 
				(SerialPort[serialPortName].RxTail + 1 ) % SP_RX_BUFF_SIZE;
				
			// Zapis odebranej danej do bufora
			udr = SerialPort[serialPortName].
				RxBuffer[SerialPort[serialPortName].RxTail];
			SerialPort[serialPortName].UsartDescriptor->ReceiveStatus =
				SerialPort_GetRcvStatus(
					*SerialPort[serialPortName].Register.rUCSRA);
		}
		else
		{
			// Timeout
			if (!timer)
			{
				SerialPort[serialPortName].UsartDescriptor->ReceiveStatus =
					SPRS_TIMEOUT_ERROR;
			}
			
			udr = -1;
		}
	}	

	return udr;
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Zwracanie liczby bajtów w buforze odbiorczym
 * @param    serialPortName : nazwa portu
 * @retval   Liczba bajtów czekających w buforze
 */
uint16_t SerialPort_CountOfBytes(ESPName_t serialPortName)
{
	return SerialPort[serialPortName].ReceivedDataLength;
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Zwracanie liczby bajtów w buforze odbiorczym
 * @param    serialPortName : nazwa portu
 * @param    offset : przesunięcie w buforze
 * @retval   Wskażnik do bufora odczytu
 */
uint8_t* SerialPort_ReadBytes(ESPName_t serialPortName, 
                              uint16_t offset)
{
	return SerialPort[serialPortName].RxBuffer + offset;
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Obsługa przerwania od odbioru
 * @param    serialPortName : nazwa portu
 * @retval   Brak
 */
void SerialPort_ReceiveHandler(ESPName_t serialPortName)
{
	uint8_t data;					// Odebrany bajt

	if (IS_SP_EXIST(serialPortName))
	{		
		// Pobieranie statusu odczytu
		SerialPort[serialPortName].UsartDescriptor->ReceiveStatus = 
			SerialPort_GetRcvStatus(*SerialPort[serialPortName].Register.rUCSRA);
		
		// Pobranie danej
		data = *SerialPort[serialPortName].Register.rUDR;
		
		// Obliczanie indeksu
		SerialPort[serialPortName].RxHead = 
			(SerialPort[serialPortName].RxHead + 1 ) % SP_RX_BUFF_SIZE;      
		
		if (SerialPort[serialPortName].RxHead == SerialPort[serialPortName].RxTail)
		{
			SerialPort[serialPortName].UsartDescriptor->ReceiveStatus = 
				SPRS_DATA_OVERRUN_ERROR;
		}
	
		SerialPort[serialPortName].RxBuffer[SerialPort[serialPortName].RxHead] = 
			data; 
	}	
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Obsługa przerwania od odbioru USART0
 * @param    Brak
 * @retval   Brak
 */
ISR(USART0_RX_IRQ)
{
	SerialPort_ReceiveHandler(SPN_USART0);
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Obsługa przerwania od odbioru USART1
 * @param    Brak 
 * @retval   Brak
 */
#ifdef SPN_USART1
ISR(USART1_RX_IRQ)
{
	SerialPort_ReceiveHandler(SPN_USART1);
}
#endif								/* SPN_USART1 */

/*----------------------------------------------------------------------------*/
/**
 * @brief    Obsługa przerwania od nadawania
 * @param    serialPortName : nazwa portu
 * @retval   Brak
 */
void SerialPort_TransmitHandler(ESPName_t serialPortName)
{
	uint8_t tmptail;				// Tymczasowy ogon

	if (IS_SP_EXIST(serialPortName))
	{
		// Sprawdzanie czy wszystkie dane zostały wysłane
		if (SerialPort[serialPortName].TxHead != 
			SerialPort[serialPortName].TxTail)
		{
			// Obliczanie indeksu bufora
			tmptail = (SerialPort[serialPortName].TxTail + 1) % SP_TX_BUFF_SIZE;
			SerialPort[serialPortName].TxTail = tmptail;      
		
			*SerialPort[serialPortName].Register.rUDR = 
				SerialPort[serialPortName].TxBuffer[tmptail]; 
		}
		else
		{
			// Wyłączenie przerwania
			*SerialPort[serialPortName].Register.rUCSRB &= 
				~_BV(SerialPort[serialPortName].Bit.bUDRIE);         
		}
	}	
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Obsługa przerwania od nadawania dla USART0
 * @param    Brak
 * @retval   Brak
 */
ISR(USART0_TX_IRQ)
{
	SerialPort_TransmitHandler(SPN_USART0);	
}

#ifdef SPN_USART1
ISR(USART1_TX_IRQ)
{
	SerialPort_TransmitHandler(SPN_USART1);
}
#endif								/* SPN_USART1 */

/*----------------------------------------------------------------------------*/
/**
 * @brief    Funkcja wysyłająca pojedynczy znak
 * @param    serialPortName : nazwa portu
 * @param    _char : dana do wysłania
 * @retval   Brak
 */
void SerialPort_TransmitChar(ESPName_t serialPortName, uint8_t _char)
{
	if (SerialPort[serialPortName].UsartDescriptor->
	IsIrqEnabled)
	{
		// Funkcja printf z użyciem przerwania
		SerialPort_SendChar_Irq(serialPortName, _char);
	}
	else
	{
		// Funkcja printf bez użycia przerwania
		SerialPort_SendChar(serialPortName, _char);
	}
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Funkcja wysyłająca tekst
 * @param    serialPortName : nazwa portu
 * @param    text : wskaźnik do tekstu
 * @retval   Brak
 */
void SerialPort_TransmitText(ESPName_t serialPortName, uint8_t* text)
{
	while(*text)
	{
		SerialPort_TransmitChar(serialPortName, *text++);
	}
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Funkcja wysyłająca do obsługi funkcji printf
 * @param    data : dana do wysłania
 * @param    stream : strumień wysyłajacy
 * @retval   Kod funkcji (0 - wszystko OK)
 */
int SerialPort_Transmit(char data, FILE* stream)
{
	if (IS_SP_EXIST(SerialPortController->PrintfPort))
	{
		SerialPort_TransmitChar(SerialPortController->PrintfPort,
		                        (uint8_t)data);	
	}	
			
	return 0;
}

/******************* (C) COPYRIGHT 2013 HENIUS *************** KONIEC PLIKU ***/