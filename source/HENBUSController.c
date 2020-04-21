/**
 *******************************************************************************
 * @file     HENBUSController.c
 * @author   HENIUS (Pawe³ Witak)
 * @version  1.1.1
 * @date     23-10-2013
 * @brief    Obs³uga transmisji po protokole HENBUS
 *******************************************************************************
 *
 * <h2><center>COPYRIGHT 2013 HENIUS</center></h2>
 */

/* Sekcja include ------------------------------------------------------------*/

// --->Pliki systemowe

#include <stdint.h>
#include <stdio.h>
#include <avr/io.h>
#include <string.h>

// --->Pliki u¿ytkownika

#include "HENBUSController.h"
#include "SerialPort.h"
#include "Utils.h"

/* Sekcja zmiennych ----------------------------------------------------------*/

/*! Ramka testowa Watchdog'a z PC */
CommProtocolFrame_t WatchdogTestFrame;
/*! Ramka odpowiedzi z zasilacza */
CommProtocolFrame_t WatchdogAnswerFrame;
ESPName_t SerialPortName;			/*! Nazwa portu szeregowego */
CommController_t Controller;		/*! Struktura kontrolera */
uint8_t DataBuffer[HENBUS_DATA_BUFF_SIZE];
#ifndef COMM_BINARY_MODE
/*! Tabela z odebran¹ komend¹ */
uint8_t CurrentCommand[HENBUS_ASCII_CMD_SIZE + 1];
#endif
/*! Aktualnie odebrana ramka */
CommProtocolFrame_t CurrentFrame =
{
#ifndef COMM_BINARY_MODE	
	.CommandName = CurrentCommand,
#endif	
	.Data        = DataBuffer
};
/*! WskaŸnik do funkcji generowanej na odebranie ramki */
void (*FrameReceivedCallback)(CommProtocolFrame_t*);
/*! Czas timeout'u (w liczbie wywo³añ funkcji obs³ugi protoko³u) */
uint16_t TimeoutTime;

/* Sekcja funkcji ------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/**
* @brief    Wysy³anie danej w formacie ASCI HEX po porcie szeregowym
* @param    wdFrame : ramka Watchdog'a
* @retval   Brak
*/
#ifndef COMM_BINARY_MODE
void HENBUSCtrl_SendAsciHexByte(uint8_t data)
{
	// Tablica na wartoœæ ASCII HEX
	uint8_t asciiHex[3] = { 0, 0 };
	
	ByteToAsciiHex(asciiHex, data);
	SerialPort_TransmitChar(SerialPortName, asciiHex[0]);
	SerialPort_TransmitChar(SerialPortName, asciiHex[1]);
}
#endif

/*----------------------------------------------------------------------------*/
/**
* @brief    Wysy³anie ramki protoko³u
* @param    frame : wskaŸnik do ramki
* @retval   Brak
*/
void HENBUSCtrl_SendFrame(CommProtocolFrame_t* frame)
{
	uint8_t index;					// Indeks pomocniczy
	// Flaga okreœlaj¹ca tryb binarny
	
	// Jeœli jest co wys³aæ
	if (frame)
	{
		// --------------------- Wysy³anie ramki -------------------------------
		
		// --->Pocz¹tek ramki - 1 bajt
		SerialPort_TransmitChar(SerialPortName, HENBUS_SOF);
		
		// --->Adres urz¹dzenia
#ifdef COMM_BINARY_MODE
		// W trybie BINARY 1 bajt
		SerialPort_TransmitChar(SerialPortName, frame->Address);
#else		
		// W trybie ASCII 2 bajty
		HENBUSCtrl_SendAsciHexByte(frame->Address);
#endif
		
		// --->Kod komendy
#ifdef COMM_BINARY_MODE
		// W trybie BINARY 1 bajt
		SerialPort_TransmitChar(SerialPortName, frame->CommandID);
#else
		// W trybie ASCII zmienna liczba bajtów 
		SerialPort_TransmitText(SerialPortName, frame->CommandName);
#endif
		
		// --->Liczba bajtów danych
#ifdef COMM_BINARY_MODE
		// W trybie BINARY 1 bajt
		SerialPort_TransmitChar(SerialPortName, frame->DataSize);
#else
		// W trybie ASCII 2 bajty
		HENBUSCtrl_SendAsciHexByte(frame->DataSize);
#endif
		
		// --->Pole danych
		if (frame->DataSize > 0)
		{
			// Wysy³anie danych - frame->DataSize * 2 bajtów
			for (index = 0; index < frame->DataSize; index++)
			{
#ifdef COMM_BINARY_MODE
				// W trybie BINARY 1 bajt * DataSize
				SerialPort_TransmitChar(SerialPortName, frame->Data[index]);
#else
				// W trybie ASCII 2 bajty * DataSize
				HENBUSCtrl_SendAsciHexByte(frame->Data[index]);
#endif
			}
			
			// --->Wysy³anie sumy kontrolnej - 2 bajty
#ifdef COMM_BINARY_MODE
			// W trybie 1 bajt
			SerialPort_TransmitChar(
				SerialPortName,
				CRC8(frame->Data, frame->DataSize));
#else
			// W trybie ASCII 2 bajty
			HENBUSCtrl_SendAsciHexByte(CRC8(frame->Data, frame->DataSize));
#endif
		}
		
		// --->Koniec ramki - 1 bajt
		SerialPort_TransmitChar(SerialPortName, HENBUS_EOF);
	}
}

/*----------------------------------------------------------------------------*/
/**
* @brief    Funkcja obs³ugi protoko³u HENBUS
* @param    Brak
* @retval   Status po³¹czenia (true - po³¹czony)
*/
bool HENBUSCtrl_Handler()
{
	// Aktualnie odebrany bajt
	uint8_t currentByte = SerialPort_ReceiveChar_Irq(SerialPortName, 0);	
	static uint8_t byteIdx = 0;		// Indeks bajtów
#ifndef COMM_BINARY_MODE
	static uint8_t AsciiHexByte[2];	// Aktualny bajt ASCII HEX
#endif
	// Indeks pocz¹tkowy i koñcowy aktualnego pola
	static int8_t currentFieldStartIndex = 0, currentFieldEndIndex = 0;
	// Indeks pocz¹tkowy i koñcowy pola danych
	static uint8_t dataStartIndex = 0, dataEndIndex = 0;
	// Indeks pocz¹tkowy i koñcowy pola CRC
	static uint8_t crcStartIndex = 0, crcEndIndex = 0;
	// Suma kontrolna aktualnej ramki
	static uint8_t crcOfFrame = 0;
	// Timer timeout'u
	static uint16_t timeoutTimer = 1;
	// Flaga okreœlaj¹ca po³¹czenie
	static bool isConnected = false;
	
	if (!--timeoutTimer)
	{
		isConnected = false;
		timeoutTimer = TimeoutTime;
	}
					
	// --->Analiza bajtów
	
	// Zapis bajt po bajcie ca³ej ramki
	if(currentByte == HENBUS_SOF ||
	   currentByte == HENBUS_EOF ||
	   (currentByte >= 0 && byteIdx))
	{
		// Wykrywanie pocz¹tku ramki
		if (currentByte == HENBUS_SOF)
		{
			// Inicjalizacja odbioru ramki
			CurrentFrame.Address = 0;
			CurrentFrame.DataSize = 0;			
			byteIdx = HENBUS_SOF_START_INDEX;
			
			// Indeksy pola adresu
			currentFieldStartIndex = HENBUS_ADDRESS_START_INDEX;
			currentFieldEndIndex = HENBUS_ADDRESS_END_INDEX;
		}
		
		// Kompletowanie ramki
		if (byteIdx >= currentFieldStartIndex &&
		    byteIdx <= currentFieldEndIndex)
		{
			// Jakie to pole?
								
			if (// --->Pole adresu
				currentFieldStartIndex == HENBUS_ADDRESS_START_INDEX ||
				// --->Pole rozmiaru danych
				currentFieldStartIndex == HENBUS_DATA_SIZE_START_INDEX ||
				// --->Pole CRC
				currentFieldStartIndex == crcStartIndex)
			{
#ifndef COMM_BINARY_MODE
				AsciiHexByte[byteIdx - currentFieldStartIndex] = 
					currentByte;
#endif					
					
				// Czy to ostatni znak pola?
				if (byteIdx == currentFieldEndIndex)
				{
					// Jakie pole?
					
					// --->Pole adresu
					if  (currentFieldStartIndex == HENBUS_ADDRESS_START_INDEX)
					{						
						CurrentFrame.Address =
#ifndef COMM_BINARY_MODE						
							AsciiHexToByte(AsciiHexByte);
#else
							currentByte;
#endif
								
						// Indeksy pola komendy
						currentFieldStartIndex = HENBUS_CMD_START_INDEX;
						currentFieldEndIndex = HENBUS_CMD_END_INDEX;
					} else
					// --->Pole rozmiaru danych
					if (currentFieldStartIndex == HENBUS_DATA_SIZE_START_INDEX)
					{		
						CurrentFrame.DataSize =
#ifndef COMM_BINARY_MODE						
							AsciiHexToByte(AsciiHexByte);
#else
							currentByte;
#endif							
											
						// Indeksy pola danych i CRC					
						if (CurrentFrame.DataSize)
						{										
							currentFieldStartIndex = dataStartIndex =
								HENBUS_DATA_OR_CRC_START_INDEX;
							currentFieldEndIndex = dataEndIndex =
								HENBUS_DATA_OR_CRC_START_INDEX + 
								CurrentFrame.DataSize
#ifndef COMM_BINARY_MODE
								* 2
#endif								
								- 1;
							crcStartIndex = dataEndIndex + 1;
							crcEndIndex = crcStartIndex + HENBUS_CRC_LENGTH - 1;
						} 
						else
						{
							// W przypadku braku danych w ramce to ju¿ koniec 
							// transmisji.
							currentFieldStartIndex = currentFieldEndIndex = 0;
						}
					} else
					// --->Pole CRC
					if (currentFieldStartIndex == crcStartIndex)
					{
						crcOfFrame =
#ifndef COMM_BINARY_MODE						
							AsciiHexToByte(AsciiHexByte);
#else
							currentByte;	
#endif												
					}
				}
			}			
			else
			// --->Pole komendy
		    if (currentFieldStartIndex == HENBUS_CMD_START_INDEX)
			{
#ifndef COMM_BINARY_MODE				
				CurrentFrame.CommandName[byteIdx - currentFieldStartIndex] =
					currentByte;
				CurrentFrame.CommandName[byteIdx - currentFieldStartIndex + 1] =
					0;
#endif					
						
				// Czy to ostatni znak pola komendy?
				if (byteIdx == currentFieldEndIndex)
				{
#ifdef COMM_BINARY_MODE
					CurrentFrame.CommandID = currentByte;
#endif					
					
					// Indeksy pola rozmiaru danych
					currentFieldStartIndex = HENBUS_DATA_SIZE_START_INDEX;
					currentFieldEndIndex = HENBUS_DATA_SIZE_END_INDEX;
				}
				
			}
			else
			// --->Pole danych
			if (currentFieldStartIndex == dataStartIndex)
			{
#ifndef COMM_BINARY_MODE				
				AsciiHexByte[(byteIdx - currentFieldStartIndex) % 2] =
					currentByte;
					
				// Czy mamy ca³y bajt?
				if ((byteIdx - currentFieldStartIndex) % 2 == 1)
				{
					CurrentFrame.Data[(byteIdx - currentFieldStartIndex) / 2] =
						AsciiHexToByte(AsciiHexByte);
				}
#else
				CurrentFrame.Data[byteIdx - currentFieldStartIndex] =
					currentByte;			
#endif				
				
				// Czy to koniec danych?
				if (byteIdx == currentFieldEndIndex)
				{
					// Indeksy pola CRC
					currentFieldStartIndex = currentFieldEndIndex + 1;
					currentFieldEndIndex = currentFieldStartIndex + 
						HENBUS_CRC_LENGTH  - 1;
				}
			}
			
		}	
			
		byteIdx++;
		
		// Sprawdzanie czy odebrano ca³¹ ramkê.
		if (currentByte == HENBUS_EOF)
		{
			byteIdx = 0;
			
			// Sprawdzanie sumy kontrolnej
			if ((CRC8(CurrentFrame.Data, CurrentFrame.DataSize) == crcOfFrame ||
			    !CurrentFrame.DataSize) &&
			    FrameReceivedCallback)
			{
				// Reset timera
				timeoutTimer = TimeoutTime;
				isConnected = true;
				
				// Czy zosta³a odebrana ramka Watchdog'a
				if (
#ifndef COMM_BINARY_MODE				
					!strcmp((char*)CurrentFrame.CommandName,
				            (char*)WatchdogTestFrame.CommandName))
#else
					CurrentFrame.CommandID == WatchdogTestFrame.CommandID)
#endif							
				{
					HENBUSCtrl_SendFrame(&WatchdogAnswerFrame);
				}
				
				FrameReceivedCallback(&CurrentFrame);				
			}
		}
	}
	
	return isConnected;
}

/*----------------------------------------------------------------------------*/
/**
* @brief    Inicjalizacja kontrolera HENBUS
* @param    wdFrame : ramka Watchdog'a od PC
* @param    wdAnswerFrame : ramka odpowiedzi na ramkê z PC
* @param    serialPortName : nazwa portu szeregowego
* @param    frameCallback : wskaŸnik do funkcji odbierania ramek
* @param    taskInterval : czas co jaki jest wywo³ywana funkcja obs³ugi
* @retval   Struktura kontrolera
*/
CommController_t HENBUSCtrl_Init(const CommProtocolFrame_t* wdTestFrame,
                                 const CommProtocolFrame_t* wdAnswerFrame,
					             ESPName_t serialPortName,
				                 void (*frameCallback)(CommProtocolFrame_t*),
								 uint16_t taskInterval) 
{
	// Inicjalizacja
	WatchdogTestFrame = *wdTestFrame;
	WatchdogAnswerFrame = *wdAnswerFrame;
	SerialPortName = serialPortName;
	TimeoutTime = HENBUS_TIMEOUT / taskInterval;
	Controller.SendFrame = HENBUSCtrl_SendFrame;
	Controller.Handler = HENBUSCtrl_Handler;
	FrameReceivedCallback = frameCallback;
	
	return Controller;
}

/******************* (C) COPYRIGHT 2013 HENIUS *************** KONIEC PLIKU ***/