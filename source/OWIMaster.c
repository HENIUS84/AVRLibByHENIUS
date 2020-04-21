/**
 *******************************************************************************
 * @file     OWImaster.c
 * @author   HENIUS (Paweł Witak)
 * @version  1.1.1
 * @date     11/06/2011
 * @brief    Obsługa magistrali 1-Wire w trybie MASTER (na podstawie noty 
             Atmel'a AVR318)
 *******************************************************************************
 *
 * <h2><center>COPYRIGHT 2011 HENIUS</center></h2>
 */

/* Sekcja include ------------------------------------------------------------*/

// --->Pliki systemowe

#include <stdint.h>
#include <stdbool.h>
#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>

// --->Pliki użytkownika

#include "OWIMaster.h"
#include "OWICrc.h"

/* Sekcja zmiennych ----------------------------------------------------------*/

uint8_t IntState;					/*!< Stan przerwań */

/* Sekcja funkcji ------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/**
 * @brief    Inicjalizacja magistrali 1-Wire (impuls RESET i PRESENCE)
 * @param    Brak
 * @retval   Stan dostępności urządzenia SLAVE (0 - brak urządzenia)
 */
bool OWIMaster_Init(void)
{
	uint8_t result;					// Rezultat funkcji
	
	SAVE_INTERRUPT();				// Wyłączanie przerwań
	
	// Impuls RESET
	OWI_PULL_BUS_LOW();				// Magistrala na LOW
	_delay_us(OWI_DELAY_H);		
		
	// Impuls PRESENCE
	OWI_RELEASE_BUS();				// Zwalnianie magistrali
	_delay_us(OWI_DELAY_I);
	result = ~OWI_PIN & OWI;
	_delay_us(OWI_DELAY_J);
	
	RESTORE_INTERRUPT();			// Przywracanie przerwań
	
	return result ? true : false;
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Zapis '1' na magistrali
 * @param    Brak
 * @retval   Brak
 */
void OWIMaster_WriteBit1(void)
{
	SAVE_INTERRUPT();				// Wyłączanie przerwań
	
	// Ściąganie magistrali do '0' i opóźnienie
	OWI_PULL_BUS_LOW();
	_delay_us(OWI_DELAY_A);
	
	// Zwalnianie magistrali i opóźneinie
	OWI_RELEASE_BUS();
	_delay_us(OWI_DELAY_B);
	
	RESTORE_INTERRUPT();			// Przywracanie przerwań
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Zapis '0' na magistrali
 * @param    Brak
 * @retval   Brak
 */
void OWIMaster_WriteBit0(void)
{
	SAVE_INTERRUPT();				// Wyłączanie przerwań
	
	// Ściąganie magistrali do '0' i opóźnienie
	OWI_PULL_BUS_LOW();
	_delay_us(OWI_DELAY_C);
	
	// Zwalnianie magistrali i opóźneinie
	OWI_RELEASE_BUS();
	_delay_us(OWI_DELAY_D);
	
	RESTORE_INTERRUPT();			// Przywracanie przerwań
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Odczyt bitu na magistrali
 * @param    Brak
 * @retval : Odczytany bit w postaci maski wyprowadzenia OWI
 */
uint8_t OWIMaster_ReadBit(void)
{
	uint8_t result;
	
	SAVE_INTERRUPT();				// Wyłączanie przerwań
	
	// Ściąganie magistrali do '0' i opóźnienie
	OWI_PULL_BUS_LOW();
	_delay_us(OWI_DELAY_A);
	
	// Zwalnianie magistrali i opóźneinie
	OWI_RELEASE_BUS();
	_delay_us(OWI_DELAY_E);
	
	// Próbkowanie magistrali i opóźnienie
	result = OWI_PIN & OWI;
	_delay_us(OWI_DELAY_F);
	
	RESTORE_INTERRUPT();			// Przywracanie przerwań
	
	return result;
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Wysłanie bajtu po 1-Wire
 * @param    byte : bajt do wysłania
 * @retval   Brak
 */
void OWIMaster_SendByte(uint8_t byte)
{
	uint8_t idx;
	
	// Wysyłanie po kolei wszystkich bitów
	for (idx = 0 ; idx < 8; idx++)
	{
		if (byte & 0x01)
		{
			OWIMaster_WriteBit1();
		}
		else
		{
			OWIMaster_WriteBit0();
		}
		
		byte >>= 1;
	}
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Odbieranie bajtu po 1-Wire
 * @param    byte : bajt do wysłania
 * @retval : Odczytany bajt
 */
uint8_t OWIMaster_ReadByte(void)
{
	uint8_t result = 0;
	uint8_t idx;
	
	// Odczyt kolejnych bitów
	for (idx = 0; idx < 8; idx++)
	{
		result >>= 1;
		
		// Odczyt bitu na magstrali
		if (OWIMaster_ReadBit())
		{
			result |= 0x80;			// Ustawianie bitu MSB
		}
	}
	
	return result;
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Odczyt zadanej liczby bajtów po 1-Wire
 * @param    *data : miejsce na odczytane dane
 * @param    length : liczba bajtów do odczytania
 * @retval   Brak
 */
void OWIMaster_ReadBytes(uint8_t *data, uint8_t length)
{
	uint8_t idx;
	
	for (idx = 0; idx < length; idx++)
	{
		*data++ = OWIMaster_ReadByte();
	}
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Zapis zadanej liczby bajtów po 1-Wire
 * @param    *data : dane do zapisu
 * @param    length : liczba bajtów do zapisania
 * @retval   Brak
 */
void OWIMaster_SendBytes(uint8_t *data, uint8_t length)
{
	uint8_t idx;
	
	for (idx = 0; idx < length; idx++)
	{
		OWIMaster_SendByte(*data++);
	}
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Wysłanie komendy SKIP ROM po 1-Wire
 * @param    Brak
 * @retval   Brak
 */
void OWIMaster_SkipRom(void)
{
	OWIMaster_SendByte(OWIC_SKIP_ROM);
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Odczyt kodu ROM (w przypadku kiedy jest jedno urządzenie SLAVE)
 * @param    *romCode : wskaźnik do kodu 
 * @retval   Poprawność sumy kontrolnej kodu (true - kod poprawny)
 */
bool OWIMaster_ReadRomCode(OWIROMCode_t *romCode)
{
	// Inicjalizacja magistrali
	OWIMaster_Init();
	
	// Wysłanie komendy READ ROM
	OWIMaster_SendByte(OWIC_READ_ROM);	
	
	// Odczyt ID urządzenia SLAVE
	OWIMaster_ReadBytes(romCode->ID, sizeof(OWIROMCode_t));
	
	// Sprawdzanie poprawności adresu	
	return OWI_CheckROMCode(romCode->ID);
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Wysłanie kodu ROM (adresowanie urządzenia)
 * @param    *romCode : wskaźnik do kodu 
 * @retval   Brak
 */
void OWIMaster_SendRomCode(OWIROMCode_t *romCode)
{
	// Inicjalizacja magistrali
	OWIMaster_Init();
	
	// Wysłanie komendy MATCH ROM
	OWIMaster_SendByte(OWIC_MATCH_ROM);
	
	// Odczyt ID urządzenia SLAVE
	OWIMaster_SendBytes(romCode->ID, sizeof(OWIROMCode_t));
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Wysłanie komendy SEARCH ROM
 * @param    *romId : wskaźnik do tablicy przechowującej aktualnie odczytany kod 
 * @param    lastDeviation : ostatnia pozycja bitowa wybrana przez algorytm
 *           (przy perwszym uruchomieniu powinna być równa 0)
 * @retval : Ostatnia pozycja bitowa, na której wystąpiła rozbieżność
 */
uint8_t OWIMaster_SearchRom(uint8_t *romId, uint8_t lastDeviation)
{
	uint8_t currentBit = 1;
    uint8_t newDeviation = 0;
    uint8_t bitMask = 0x01;
    uint8_t bitA;
    uint8_t bitB;
	
	// Wysłanie komendy SEARCH ROM
    OWIMaster_SendByte(OWIC_ROM_SEARCH);	
    
    // Analiza wszsytkich 64 bitów
    while (currentBit <= 64)
    {
        // Odczyt bitu z magistrali podwójnie
        bitA = OWIMaster_ReadBit();
        bitB = OWIMaster_ReadBit();

        if (bitA && bitB)
        {
            // Obydwa bity == 1 (błąd)
            return 0;
        }
        else if (bitA ^ bitB)
        {
            // Wszystkie urządzenia mają te same bity na tej pozycji.
            if (bitA)
            {
                (*romId) |= bitMask;
            }
            else
            {
                (*romId) &= ~bitMask;
            }
        }
        else						// Obydwa bity równe 0
        {            
            if (currentBit == lastDeviation)
            {
				// Jeśli to ostatniow wybrana pozycja, 
				// '1' będzie wybrana tym razem.
                (*romId) |= bitMask;
            }
            else if (currentBit > lastDeviation)
            {
				// Dla pozostałych ID '0' wybrane kiedy występują rozbieżności.
                (*romId) &= ~bitMask;
                newDeviation = currentBit;
            }            
            else if ( !(*romId & bitMask)) 
            {
				// Szukanie nowej dewiacji
                newDeviation = currentBit;
            }
        }               
        
        // Wysłanie wybranego bitu na magistralę
        if ((*romId) & bitMask)
        {
            OWIMaster_WriteBit1();
        }
        else
        {
            OWIMaster_WriteBit0();
        }
          
        currentBit++;				// Analiza kolejnego bitu

        // Ustalanie wskaźnika bitMask i bitPattern    
        bitMask <<= 1;
        if (!bitMask)
        {
            bitMask = 0x01;
            romId++;
        }
    }
	
    return newDeviation;
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Sprawdzanie czy kod rodziny znajduje się na liście
 * @param    *devices : wskaźnik do tablicy przechowującej wszystkie wyszukane
 *           Id 
 * @param    familyCode : poszukiwany kod rodziny
 * @param    *codes : lista kodów rodziny urządzeń do wyszukiwania
 *           (element 0xFF oznacza koniec listy)
 * @retval : Rezultat poszukiwania (true - od jest na liście)
 */
bool OWIMaster_IsFamilyCodeExist(EOWIFamily_t *codes, EOWIFamily_t familyCode)
{
	bool result = false;
	
	while (*codes != OWIF_EOL)
	{
		if (*codes == familyCode ||
		    *codes == OWIF_ALL)
		{
			result = true;
			break;
		}
		
		codes++;
	}
	
	return result;
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Wyszukiwanie urządzeń SLAVE na 1-Wire
 * @param    *devices : wskaźnik do tablicy przechowującej wszystkie wyszukane
 *           Id 
 * @param    length : maksymalna liczba urządzeń do wyszukania
 * @param    *familyCodes : lista kodów rodziny urządzeń do wyszukiwania
 *           (0 - wyszukiwanie wszystkich)
 * @retval : Liczba odnalezionych urządzeń (0 - niepowodzenie, jeśli jakieś 
             urządzenie jest podłączone oznacza błąd transmisji)
 */
uint8_t OWIMaster_SearchBuses(OWIDevice_t *devices, 
                              uint8_t length,
							  EOWIFamily_t *familyCodes)
{
	uint8_t i, j;
    uint8_t lastDeviation;
    uint8_t numDevices = 0;
	uint8_t tempId[9];
    
    // Inicjalizacja wszystkich adresów na 0    
    for (i = 0; i < length; i++)
    {
        for (j = 0; j < 8; j++)
        {
            devices[i].ROMCode.ID[j] = 0x00;
        }
    }
    
    // Szukanie urządzeń
    lastDeviation = 0;
    if (OWIMaster_Init())			// Urządzenia dostępne na magistrali
    {
		// Umieszczanie odnalezionych adresów w tablicy
        do  
        {
            OWIMaster_Init();
            lastDeviation = OWIMaster_SearchRom(tempId, 
			                                    lastDeviation);
												
			if (OWIMaster_IsFamilyCodeExist(familyCodes, 
			                                tempId[0]))
			{
				// Sprawdzanie poprawności CRC adresu
				if (OWI_CheckROMCode(tempId))
				{					
					memcpy(devices[numDevices].ROMCode.ID, tempId, 8);
					devices[numDevices].IsExist = true;
					numDevices++;
				}				
			}												
        }  while (numDevices < length && lastDeviation);            
    }
	
	return numDevices;	
}

/******************* (C) COPYRIGHT 2011 HENIUS *************** KONIEC PLIKU ***/
