/**
 *******************************************************************************
 * @file     SerialPort.h
 * @author   HENIUS (Paweł Witak)
 * @version  1.01.003
 * @date     15/11/2013
 * @brief    Zestaw funkcji związanych z obsługą portu szeregowego z IRQ
 *           (plik nagłówkowy)
 *******************************************************************************
 *
 * <h2><center>COPYRIGHT 2013 HENIUS</center></h2>
 */

#ifndef  SERIAL_PORT_H
#define  SERIAL_PORT_H

/* Sekcja include ------------------------------------------------------------*/

// --->Pliki systemowe

#include <stdbool.h>
#include <stdint.h>

/* Sekcja stałych, makr i definicji ------------------------------------------*/

//--->Stałe

#define SP_TX_BUFF_SIZE (128)       /*!< Rozmiar bufora nadawczego */
#if (SP_TX_BUFF_SIZE & (SP_TX_BUFF_SIZE - 1))
#error Rozmiar bufora nadawczego powinien byc potcga 2
#endif
#define SP_RX_BUFF_SIZE (128)       /*!< Rozmiar bufora odbiorczego */
#if (SP_RX_BUFF_SIZE & (SP_RX_BUFF_SIZE - 1))
#error Rozmiar bufora odbiorczego powinien byc potega 2
#endif

/* Sekcja deklaracji ---------------------------------------------------------*/

// --->Stałe i makra

/*! Makro sprawdzające istnienie portu */
#define IS_SP_EXIST(port)	(port < SP_NUMBER_OF_PORTS)

#ifdef UDR1
/*! Procek zawiera moduł USART1 */
#define USART1
#endif

// Opis rejestrów i flag
#ifndef USART0_TX_vect
/*! Wektora przerwania od nadawania USART */
#define USART0_TX_IRQ		(USART_UDRE_vect)
#else
/*! Wektora przerwania od nadawania USART0 */
#define USART0_TX_IRQ		(USART0_UDRE_vect)
#endif

#ifndef USART0_RX_vect
/*! Wektora przerwania od odbioru USART */
#define USART0_RX_IRQ		(USART_RXC_vect)
#else
/*! Wektora przerwania od odbioru USART0 */
#define USART0_RX_IRQ		(USART0_RX_vect)
#endif

#ifndef UDR0
/*! Deklaracja rejestru UDR */
#define UDR_0				(UDR)
#else
/*! Deklaracja rejestru UDR0 */
#define UDR_0				(UDR0)
#endif

#ifndef UCSR0A
/*! Deklaracja rejestru UCSRA */
#define UCSRA_0				(UCSRA)
#else
/*! Deklaracja rejestru UCSR0A */
#define UCSRA_0				(UCSR0A)
#endif

#ifndef UCSR0B
/*! Deklaracja rejestru UCSRB */
#define UCSRB_0				(UCSRB)
#else
/*! Deklaracja rejestru UCSR0B */
#define UCSRB_0				(UCSR0B)
#endif

#ifndef UCSR0C
/*! Deklaracja rejestru UCSRC */
#define UCSRC_0				(UCSRC)
#else
/*! Deklaracja rejestru UCSR0C */
#define UCSRC_0				(UCSR0C)
#endif

#ifndef UBRR0H
/*! Deklaracja rejestru UBRRH */
#define UBRRH_0				(UBRRH)
#else
/*! Deklaracja rejestru UBRR0H */
#define UBRRH_0				(UBRR0H)
#endif

#ifndef UBRR0L
/*! Deklaracja rejestru UBRRL */
#define UBRRL_0				(UBRRL)
#else
/*! Deklaracja rejestru UBRR0L */
#define UBRRL_0				(UBRR0L)
#endif

#ifndef FE0
/*! Deklaracja flagi FE */
#define FE_0				(FE)
#else
/*! Deklaracja flagi FE0 */
#define FE_0				(FE0)
#endif

#ifndef DOR0
/*! Deklaracja flagi DOR */
#define DOR_0				(DOR)
#else
/*! Deklaracja flagi DOR0 */
#define DOR_0				(DOR0)
#endif

#ifndef UPE0
/*! Deklaracja flagi UPE */
#define UPE_0				(PE)
#else
/*! Deklaracja flagi UPE0 */
#define UPE_0				(UPE0)
#endif

#ifndef UCSZ00
/*! Deklaracja flagi UCSZ0 */
#define UCSZ0_0				(UCSZ0)
#else
/*! Deklaracja flagi UCSZ00 */
#define UCSZ0_0				(UCSZ00)
#endif

#ifndef UCSZ01
/*! Deklaracja flagi UCSZ1 */
#define UCSZ1_0				(UCSZ1)
#else
/*! Deklaracja flagi UCSZ01 */
#define UCSZ1_0				(UCSZ01)
#endif

#ifndef UCSZ02
/*! Deklaracja flagi UCSZ2 */
#define UCSZ2_0				(UCSZ2)
#else
/*! Deklaracja flagi UCSZ02 */
#define UCSZ2_0				(UCSZ02)
#endif

#ifndef USBS0
/*! Deklaracja flagi USBS */
#define USBS_0				(USBS)
#else
/*! Deklaracja flagi USBS0 */
#define USBS_0				(USBS0)
#endif

#ifndef UPM00
/*! Deklaracja flagi UPM0 */
#define UPM0_0				(UPM0)
#else
/*! Deklaracja flagi UPM00 */
#define UPM0_0				(UPM00)
#endif

#ifndef UMSEL0
/*! Deklaracja flagi UMSEL */
#define UMSEL_0				(UMSEL)
#else
/*! Deklaracja flagi UMSEL0 */
#define UMSEL_0				(UMSEL0)
#endif

#ifndef UCPOL0
/*! Deklaracja flagi UCPOL */
#define UCPOL_0				(UCPOL)
#else
/*! Deklaracja flagi UCPOL0 */
#define UCPOL_0				(UCPOL0)
#endif

#ifndef U2X0
/*! Deklaracja flagi U2X */
#define U2X_0				(U2X)
#else
/*! Deklaracja flagi U2X0 */
#define U2X_0				(U2X0)
#endif

#ifndef RXEN0
/*!< Deklaracja flagi RXEN */
#define RXEN_0				(RXEN)
#else
/*! Deklaracja flagi RXEN0 */
#define RXEN_0				(RXEN0)
#endif

#ifndef TXEN0
/*! Deklaracja flagi TXEN */
#define TXEN_0				(TXEN)
#else
/*! Deklaracja flagi TXEN0 */
#define TXEN_0				(TXEN0)
#endif

#ifndef TXCIE0
/*! Deklaracja flagi TXCIE */
#define TXCIE_0				(TXCIE)
#else
/*! Deklaracja flagi TXCIE0 */
#define TXCIE_0				(TXCIE0)
#endif

#ifndef RXCIE0
/*! Deklaracja flagi RXCIE */
#define RXCIE_0				(RXCIE)
#else
/*! Deklaracja flagi RXCIE0 */
#define RXCIE_0				(RXCIE0)
#endif

#ifndef RXC0
/*! Deklaracja flagi RXC */
#define RXC_0				(RXC)
#else
/*! Deklaracja flagi RXC0 */
#define RXC_0				(RXC0)
#endif

#ifndef UDRE0
/*! Deklaracja flagi UDRE */
#define UDRE_0				(UDRE)
#else
/*! Deklaracja flagi UDRE0 */
#define UDRE_0				(UDRE0)
#endif

#ifndef UDRIE0
/*! Deklaracja flagi UDRIE */
#define UDRIE_0				(UDRIE)
#else
/*! Deklaracja flagi UDRIE0 */
#define UDRIE_0				(UDRIE0)
#endif

// --->Typy

/**
 * @brief Nazwy portów szeregowych
 */
typedef enum 
{
   SPN_USART0, 						/*!< Port USART0 */
#ifdef UDR1
   SPN_USART1,						/*!< Port USART1 */
#endif
   SP_NUMBER_OF_PORTS				/*!< Liczba portów */
}ESPName_t;

/**
 * @brief Struktura inicjalizująca komunikację szeregową
 */
typedef struct
{
	void(*Delay)(uint16_t time);	/*!< Wskaźnik do funkcji opóźniającej */
	uint32_t CpuFrequency;			/*!< Częstotliwość CPU w MHz */
	bool IsPrintfEnabled;			/*!< Flaga aktywująca funkcję printf */
	ESPName_t PrintfPort;			/*!< Nazwa portu dla funkcji printf */
}SPController_t;

/**
 * @brief Rodzaje parzystości
 */
typedef enum 
{
   SPP_NO_PARITY = 0, 				/*!< Brak parzystości */
   SPP_EVEN      = 2, 				/*!< Parzystość */
   SPP_ODD       = 3				/*!< Nieparzystość */
}SPParity_t;

/**
 * @brief Tryb zbocza sygnału
 */
typedef enum  
{
   SPE_TRRF,                        /*!< TxD - opadające, RxD - narastajace */
   SPE_TFRR                         /*!< TxD - narastające, RxD - opadające */
}ESPEdge_t; 

/**
 * @brief Tryb pracy (synchroniczny/asynchroniczny)
 */
typedef enum  
{
   SPSM_ASYNCHRONOUS, 				/*!< Asynchroniczny */
   SPSM_SYNCHRONOUS					/*!< Synchroniczny */
}ESPSyncMode_t;

/**
 * @brief Tryb prędkości (normalna/podwójna)
 */
typedef enum  
{
   SPSM_NORMAL, 					/*!< Normalna prędkość */
   SPSM_DOUBLE						/*!< Podwójna prędkość */
}ESPSpeedMode_t; 

/**
 * @brief Status odczytu
 */
typedef enum
{
	SPRS_OK,						/*!< Transmisja bez błędów */
	SPRS_PARITY_ERROR,				/*!< Błąd parzystości */
	SPRS_DATA_OVERRUN_ERROR,		/*!< Nadpisanie danych */
	SPRS_FRAME_ERROR, 				/*!< Błąd ramki */
	SPRS_TIMEOUT_ERROR				/*!< Błąd timeout'u */
}ESPRcvStatus_t;

/**
 * @brief Prędkości transmisji
 */
typedef enum
{
	/*! 4800 b/s */
	SPBR_4800   = 4800,		
	/*! 9600 b/s */
	SPBR_9600   = 9600,		
	/*! 19200 b/s */
	SPBR_19200  = 19200,	
	/*! 38400 b/s */
	SPBR_38400  = 38400,	
	/*! 57600 b/s */
	SPBR_57600  = 57600,	
	/*! 115200 b/s */
	SPBR_115200 = 115200	
}ESPBaudRate_t;

/**
 * @brief Struktura opisująca port szeregowy
 */
typedef struct
{
   ESPBaudRate_t BaudRate;          /*!< Prędkość transmisji w b/s */
   uint8_t DataLength;				/*!< Długość danych (5-8) */
   uint8_t StopBits;                /*!< Bity stopu (1 lub 2) */
   SPParity_t Parity;               /*!< Parzystośc */
   /*! Tryb zbocza sygnału (opadające/narastające) */
   ESPEdge_t Edge;                  
   ESPSyncMode_t SyncMode;          /*!< Tryb pracy 
                                         (synchroniczny/asynchroniczny) */
   ESPSpeedMode_t SpeedMode;        /*!< Tryb predkości */
   ESPRcvStatus_t ReceiveStatus;	/*!< Status odczytu */
   bool IsIrqEnabled;				/*!< Flaga oznaczająca aktywację przerwań */
}SPDescriptor_t;

/**
 * @brief Opis portu szeregowego
 */
typedef struct  
{
	/*! Bufor odbiorczy */
	uint8_t RxBuffer[SP_RX_BUFF_SIZE];	
	/*! Bufor nadawczy */
	uint8_t TxBuffer[SP_TX_BUFF_SIZE];
	uint8_t RxHead;					/*!< Głowa bufora odbiorczego */
	uint8_t RxTail;					/*!< Ogon bufora odbiorczego */
	uint8_t TxHead;					/*!< Głowa bufora nadawczego */
	uint8_t TxTail;					/*!< Ogon bufora nadawczego */
	struct
	{
		volatile uint8_t *rUCSRA;	/*!< Rejestr kontrolny UCSRA */
		volatile uint8_t *rUCSRB;	/*!< Rejestr kontrolny UCSRB */
		volatile uint8_t *rUCSRC;	/*!< Rejestr kontrolny UCSRC */
		volatile uint8_t *rUDR;		/*!< Rejestr danych UDR */
		volatile uint8_t *rUBRRL;	/*!< Rejestr danych UBRRL */
		volatile uint8_t *rUBRRH;	/*!< Rejestr danych UBRRH */
	}Register;						/*!< Rejestry */
	struct 
	{
		uint8_t bRXC;				/*!< Bit RXC */
		uint8_t bRXCIE;				/*!< Bit RXCIE */
		uint8_t bTXCIE;				/*!< Bit TXCIE */
		uint8_t bUCSZ0;				/*!< Bit UCSZ0 */	
		uint8_t bUCSZ2;				/*!< Bit UCSZ2 */
		uint8_t bUPM0;				/*!< Bit UPM0 */
		uint8_t bUSBS;				/*!< Bit USBS */	
		uint8_t bUMSEL;				/*!< Bit UMSEL */	
		uint8_t bUCPOL;				/*!< Bit UCPOL */
		uint8_t bU2X;				/*!< Bit U2X */		
		uint8_t bRXEN;				/*!< Bit RXEN */	
		uint8_t bTXEN;				/*!< Bit TXEN */
		uint8_t bUDRE;				/*!< Bit UDRE */
		uint8_t bUDRIE;				/*!< Bit UDRIE */
	}Bit;							/*!< Bity */
	SPDescriptor_t *UsartDescriptor;/*!< Opis portu (wskaźnik) */
	bool IsPortOpen; 				/*!< Flaga oznaczająca otwarty port */
	uint8_t ReceivedDataLength;		/*!< Liczba bajtów odebranych danych */
}SerialPort_t;

// --->Funkcje
// Inicjalizacja portu szeregowego
void SerialPort_Init(SPController_t *data);
// Otwarcie portu
void SerialPort_Open(ESPName_t serialPortName,
                     SPDescriptor_t *serialPortConfig);
// Zamknięcie portu
void SerialPort_Close(ESPName_t serialPortName);
// Wysłanie znaku bez przerwania
void SerialPort_SendChar(ESPName_t serialPortName,
		                 uint8_t character);
// Wysłanie znaku z przerwaniem
void SerialPort_SendChar_Irq(ESPName_t serialPortName,
		                     uint8_t character);
// Zwracanie liczby bajtów w buforze odbiorczym
uint16_t SerialPort_CountOfBytes(ESPName_t serialPortName);
// Zwracanie liczby bajtów w buforze odbiorczym
uint8_t* SerialPort_ReadBytes(ESPName_t serialPortName,
                              uint16_t offset);			 
// Czekanie na odebranie znaku (bez przerwania)
int16_t SerialPort_ReceiveChar(ESPName_t serialPortName, uint16_t timeout);
// Czekanie na odebranie znaku (z przerwaniem)
int16_t SerialPort_ReceiveChar_Irq(ESPName_t serialPortName, uint16_t timeout);
// Funkcja wysyłająca pojedynczy znak
void SerialPort_TransmitChar(ESPName_t serialPortName, uint8_t _char);
// Funkcja wysyłająca tekst
void SerialPort_TransmitText(ESPName_t serialPortName, uint8_t* text);

#endif								/* SERIAL_PORT_H */

/******************* (C) COPYRIGHT 2013 HENIUS *************** KONIEC PLIKU ***/