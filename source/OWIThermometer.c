/**
 *******************************************************************************
 * @file     OWIThermometermeter.c
 * @author   HENIUS (Paweł Witak)
 * @version  1.1.2
 * @date     10/04/2013
 * @brief    Obsługa czujnika temperatury na 1-Wire
 *******************************************************************************
 *
 * <h2><center>COPYRIGHT 2013 HENIUS</center></h2>
 */

/* Sekcja include ------------------------------------------------------------*/

// --->Pliki systemowe

#include <stdint.h>
#include <stdbool.h>
#include <util/delay.h>

// --->Pliki użytkownika

#include "OWIMaster.h"
#include "OWIThermometer.h"
#include "OWICrc.h"

/* Sekcja zmiennych,makr i stałych -------------------------------------------*/

// --->Zmienne

OWIThermoCtrl_t *Thermometers;	/*!< Wskaźnik do tablicy urządzeń 1-Wire */
/*!< Timer oczekiwania na zakończenie konwersji temperatury */
uint16_t ThermDelayTimer;	
/*! Czas oczekiwania na zakończenie konwersji temperatury */	
uint16_t ThermoDelay;			
/*!< Timer oczekiwania na rozpoczęcie kolejnego pomiaru temperatury */
uint16_t ThermoIntervalTimer;
uint16_t ThermoInterval;			/*!< Czas oczekiwania na kolejny pomiar */
/*! Lista kodów rodziny termometrów */
EOWIFamily_t ThermoDeviceFC[] =		
{
	OWIF_DS18B20,
	OWIF_DS18S20,
	OWIF_DS1822,
	OWIF_EOL
};

/* Sekcja funkcji ------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/**
 * @brief    Inicjalizacja czujnika temperatury na 1-Wire
 * @param    *thermometers : wskaźnik tablicy czujników temperatury
 * @retval   Brak
 */
void OWIThermo_Init(OWIThermoCtrl_t *thermometers)
{
	// Pamięć na konfigurację
	OWIThermoConfig_t configuration;		
	
	Thermometers = thermometers;
		
	// Pobieranie adresów urządzeń
	if (Thermometers->MaxAmountOfDevices == 1)
	{
		// Na magistrali jest tylko jeden czujnik.
		if (OWIMaster_ReadRomCode(&Thermometers->Devices[0].ROMCode) &&
		    OWIMaster_IsFamilyCodeExist(
				ThermoDeviceFC, 
			    Thermometers->Devices[0].ROMCode.Family))
		{ 			
			Thermometers->AmountOfFoundDevices = 1;
		}
	}
	else if (Thermometers->MaxAmountOfDevices > 1)
	{
		Thermometers->AmountOfFoundDevices = 
			OWIMaster_SearchBuses(Thermometers->Devices, 
			                      Thermometers->MaxAmountOfDevices, 
								  ThermoDeviceFC);								  
	}
	
	// Przygotowanie do ustawiania rozdzielczości pomiaru
	OWIMaster_SkipRom();
	configuration.ConfigReg.Resolution = Thermometers->Resolution;
	ThermoDelay = OWI_TEMP_CONV_TIME / (0x08 >> Thermometers->Resolution);
	if (Thermometers->RepetitionTime <= ThermoDelay)
	{
		ThermoInterval = Thermometers->TaskTime;
	}
	else
	{
		ThermoInterval = Thermometers->RepetitionTime - ThermoDelay;
	}
	
	// Ustawianie timerów
	ThermoDelay /= Thermometers->TaskTime;
	ThermoInterval /= Thermometers->TaskTime;
	ThermDelayTimer = ThermoDelay;
	ThermoIntervalTimer = ThermoInterval;
	
	// Właściwy zapis konfiguracji
	OWIThermo_WriteConfig(&configuration, 0);	
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Konwersja temperatury
 * @param    *thermMem : wskaźnik do pamięci czujnika
 * @param    family : typ czujnika
 * @retval   Temperatura (z uwzględnieniem znaku)
 */
int16_t OWITherm_ConvertTemp(OWIThermoMem_t *thermMem, EOWIFamily_t family)
{
	uint16_t bitMask = 0;			// Maska dla temperatury
	
	switch (family)
	{
		// --->Czujniki programowalne
		case OWIF_DS18B20:
		case OWIF_DS1822:
			bitMask = 0x07FF;
		
			break;
		
		// --->Czujniki o wysokiej rozdzielczości
		case OWIF_DS18S20:
			bitMask = 0x00FF;
		
			break;
			
		default:
			break;
	}
	
	return 
		(thermMem->Temperature & bitMask) * 
		((thermMem->Temperature & 0x8000) ? -1 : 1);
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Odczyt pamięci czujnika
 * @param    *memory : wskaźnik do struktury pamięci
 * @oaram	 *romId : wskaźnik do adresu czujnika (0 - to Skip Rom)
 * @retval   Poprawność sumy kontrolnej pamięci (true - odczyt bez błędów)
 */
bool OWIThermo_ReadMemory(OWIThermoMem_t *memory, OWIROMCode_t *romId)
{
	bool result = false;			// Wynik funkcji
	
	// Inicjalizacja magistrali
	if (OWIMaster_Init())
	{
		// Adresowanie czujnika
		if (!romId)
		{
			OWIMaster_SkipRom();
		}
		else
		{
			OWIMaster_SendRomCode(romId);
		}
		
		// Właściwy zapis konfiguracji (zależne od typu czujnika)
		OWIMaster_SendByte(OWITC_READ_MEM);
		OWIMaster_ReadBytes(memory->All, sizeof(OWIThermoMem_t));
		
		result = !OWI_CalculateCRC(memory->All, sizeof(OWIThermoMem_t)) ?
			true : false;
	}
	
	return result;
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Funkcja zapisu konfiguracji
 * @param    *config : wskaźnik konfiguracji
 * @oaram	 *romId : wskaźnik do adresu czujnika (0 - to Skip Rom)
 * @retval   Brak
 */
void OWIThermo_WriteConfig(OWIThermoConfig_t *config, OWIROMCode_t *romId)
{
	// Inicjalizacja magistrali
	if (OWIMaster_Init())
	{
		// Adresowanie czujnika
		if (!romId)
		{
			OWIMaster_SkipRom();
		}
		else
		{
			OWIMaster_SendRomCode(romId);
		}
		
		// Właściwy zapis konfiguracji (zależne od typu czujnika)
		OWIMaster_SendByte(OWITC_WRITE_MEM);
		OWIMaster_SendBytes(config->All, romId->Family == OWIF_DS18S20 ? 
			sizeof(OWIThermoConfig_t) - 1 : sizeof(OWIThermoConfig_t));
	}
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Odzcyt temperatury danego czujnika
 * @param    *romID : kod czujnika
 * @retval   Temperatura czujnika
 */
int16_t OWIThermo_ReadTemperature(OWIROMCode_t *romID)
{
	OWIMaster_Init();				// Inicjalizacja magistrali
	OWIThermoMem_t thermMem;		// Pamięć czujnika
	int16_t temperature = 0;		// Odczytana temperatura
	
	// W przypadku niepodania ID czujnika wysyłana jest komenda SKIM ROM.
	if (!romID)
	{
		OWIMaster_SkipRom();		// Pominięcie adresu
		
	}
	else
	{
		// Podawanie adresu czujnika
		OWIMaster_SendRomCode(romID);
	}
	
	// Rozpoczęcie konwersji i czekanie na jej zakończenie
	OWIMaster_SendByte(OWITC_START_CONV);
	Thermometers->Delay_ms(ThermoDelay);
	
	// Odczyt pamięci
	if (OWIThermo_ReadMemory(&thermMem, romID))
	{
		temperature = OWITherm_ConvertTemp(&thermMem, romID->Family);
	}
	
	return temperature;
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Funkcja obsługi pomiaru temperatury
 * @param    Brak
 * @retval   Brak
 */
void OWIThermo_Handler(void)
{
	// Aktualny stan maszyny stanów
	static EOWIThermoState_t currStates = OWITS_WAIT_FOR_NEXT;
	// Flaga oznaczająca czy należy czytać pamięć podręczną
	static bool readScratchpad = false;	
	static uint8_t devNumber;		// Numer urządzenia
	static uint8_t byteNumber;		// Numer odczytywanego bajtu	
	// Pamięć czujnika temperatury
	static OWIThermoMem_t thermMem;	
	// Liczba urządzeń	
	uint8_t amountOfDevice = 
		Thermometers->AmountOfFoundDevices >= Thermometers->MaxAmountOfDevices ?
		Thermometers->MaxAmountOfDevices : Thermometers->AmountOfFoundDevices;
	
	switch (currStates)
	{
		// Inicjalizacja pomiaru
		case OWITS_INIT:
			currStates = OWITS_SKIP_ROM;						
			OWIMaster_Init();		// Inicjalizacja magistrali 1-Wire
									
			break;
			
		// Pomijanie ROM kodu
		case OWITS_SKIP_ROM:
			if (!readScratchpad)
			{
				currStates = OWITS_START_CONV;
				OWIMaster_SkipRom();	// Komenda SKIP ROM	
			}
			else
			{
				if (amountOfDevice == 1)
				{
					currStates = OWITS_READ_MEM_REQUEST;
					OWIMaster_SkipRom();// Komenda SKIP ROM	
				}
				else if (amountOfDevice > 1)
				{
					currStates = OWITS_MATCH_ROM;
				}
			}					
		
			break;
			
		// Żądanie konwersji
		case OWITS_START_CONV:
			currStates = OWITS_WAIT;	
			// Rozpoczęcie konwersji	
			OWIMaster_SendByte(OWITC_START_CONV);	
					
			break;
			
		// Czekanie na zakończenie konwersji
		case OWITS_WAIT:
			if (!(--ThermDelayTimer))
			{
				// Czas minął, więc można przejść do kolejnego stanu i 
				// roczpocząć proces odczytywania danych								
				currStates = OWITS_INIT;				
				ThermDelayTimer = ThermoDelay;;
				readScratchpad = true;
			}
		
			break;
		
		// Wysyłanie adresu urządzenia
		case OWITS_MATCH_ROM:
			OWIMaster_SendRomCode(&Thermometers->Devices[devNumber].ROMCode);
			Thermometers->Devices[devNumber].IsExist = true;
			currStates = OWITS_READ_MEM_REQUEST;
		
			break;
		
		// Żądanie odczytu temperatury
		case OWITS_READ_MEM_REQUEST:
			currStates = OWITS_READ_MEM;
			OWIMaster_SendByte(OWITC_READ_MEM);
			byteNumber = 0;
			readScratchpad = false;		
		
			break;
		
		// Odczyt pamięci podręcznej	
		case OWITS_READ_MEM:
			currStates = OWITS_READ_MEM;
			thermMem.All[byteNumber] = OWIMaster_ReadByte(); 			
			byteNumber++;
			
			if (byteNumber == sizeof(OWIThermoMem_t))
			{
				ThermDelayTimer = ThermoDelay;
				
				// Liczenie CRC
				Thermometers->Devices[devNumber].IsExist = 
					!OWI_CalculateCRC(thermMem.All, sizeof(OWIThermoMem_t)) ? 
						true : false;
				
				if (Thermometers->Devices[devNumber].IsExist)
				{		
					Thermometers->Temperatures[devNumber] = 
						OWITherm_ConvertTemp( 
							&thermMem, 
						    Thermometers->Devices[devNumber].ROMCode.Family);								
				}
				
				if (devNumber == (amountOfDevice - 1))
				{
					currStates = OWITS_WAIT_FOR_NEXT;
					devNumber = 0;
				}
				else
				{
					currStates = OWITS_INIT;
					readScratchpad = true;
					devNumber++;
				}	
			}
		
			break;	
			
		// Oczekiwanie na kolejny pomiar
		case OWITS_WAIT_FOR_NEXT:
			if (!(--ThermoIntervalTimer))
			{	
				// Czas minął, więc można przejść do kolejnego stanu.
				currStates = OWITS_INIT;
				ThermoIntervalTimer = ThermoInterval;				
			}
		
			break;
		
		// Nieznany stan	
		default:
			currStates = OWITS_INIT;
		
			break;
	}
}

/******************* (C) COPYRIGHT 2013 HENIUS *************** KONIEC PLIKU ***/
