/**
 *******************************************************************************
 * @file     OWIMaster.h
 * @author   HENIUS (Paweł Witak)
 * @version  1.01.001
 * @date     11-06-2011
 * @brief    Obsługa magistrali 1-Wire w trybie MASTER (plik nagłówkowy)
 *******************************************************************************
 *
 * <h2><center>COPYRIGHT 2011 HENIUS</center></h2>
 */

#ifndef OWI_MASTER_H_
#define OWI_MASTER_H_

/* Sekcja include ------------------------------------------------------------*/

// --->Pliki systemowe

#include <stdbool.h>
#include <stdint.h>

/* Sekcja stałych, makr i definicji ------------------------------------------*/

// Konfiguracja magistrali
// Wyprowadzenia
#define OWI_PORT			(PORT?)	/*!< Rejestr wyjściowy 1-Wire */
#define OWI_DDR				(DDR?)	/*!< Rejestr kierunkowy 1-Wire */
#define OWI_PIN				(PIN?)	/*!< Rejestr wyjściowy 1-Wire */
#define OWI					(_BV(?))/*!< Maska wyprowadzenia 1-Wire */
//#define OWI_INT_PULLUP			/*!< Aktywacja wewnętrznego pullup'a */
/*! Zwalnianie magistrali - pin w stanie wejściowym */
#ifdef OWI_INT_PULLUP
#define OWI_RELEASE_BUS()	OWI_DDR &= ~OWI;\
                            OWI_PORT |= OWI
#else
#define OWI_RELEASE_BUS()	OWI_DDR &= ~OWI;\
                            OWI_PORT &= ~OWI
#endif
/*! Ściąganie magitrali 1-Wire do masy */
#define OWI_PULL_BUS_LOW()	OWI_DDR |= OWI;\
						    OWI_PORT &= ~OWI
							
// Użyteczne makra
/*! Zapis stanu przerwań oraz ich wyłączanie */
#define SAVE_INTERRUPT()	IntState = SREG;\
                            cli()
/*! Przywracanie przerwań */
#define RESTORE_INTERRUPT()	SREG = IntState

// --->Typy

/**
 * @brief Kody rodziny urządzeń 1-Wire
 */
typedef enum
{
	// Termometry
	/*! Cyfrowy termometr o programowalnej rozdzelczości */
	OWIF_DS18B20 = 0x28,	
	/*! Cyfrowy termometr o wysokiej precyzji */		
	OWIF_DS18S20 = 0x10,
	/*! Cyfrowy termometr o programowalnej rozdzelczości */
	OWIF_DS1822  = 0x22,
	
	/*! Dowolne urządzenie */
	OWIF_ALL = 0xFE,
	/*! Koniec listy */
	OWIF_EOL
}EOWIFamily_t;

/**
 * @brief Opis ID urządzenia 1-Wire
 */
typedef union
{
	struct
	{		
		/*! Kod rodziny */
		EOWIFamily_t Family : 8;
		/*! Numer seryjny */
		uint8_t SerialNumber[6];
		/*! Suma kontrolna */
		uint8_t	Crc;
	};	
	
	uint8_t ID[8];					/*!< Tablica całego adresu */	
}OWIROMCode_t;

/**
 * @brief Urządzenie 1-Wire
 */
typedef struct
{
	OWIROMCode_t ROMCode;			/*!< Id urządzenia */
	bool IsExist;					/*!< Flaga poprawności transmisji */
}OWIDevice_t;

// --->Stałe

// Stałe czasowe (w us)
#define OWI_DELAY_A			(6)		/*< Stała czasowa typu A */   
#define OWI_DELAY_B			(64)	/*< Stała czasowa typu B */ 
#define OWI_DELAY_C			(60)	/*< Stała czasowa typu C */ 
#define OWI_DELAY_D			(10)	/*< Stała czasowa typu D */ 
#define OWI_DELAY_E			(9)		/*< Stała czasowa typu E */ 
#define OWI_DELAY_F			(55)	/*< Stała czasowa typu F */ 
#define OWI_DELAY_G			(0)		/*< Stała czasowa typu G */
#define OWI_DELAY_H			(480)	/*< Stała czasowa typu H */ 
#define OWI_DELAY_I			(70)	/*< Stała czasowa typu I */ 
#define OWI_DELAY_J			(410)	/*< Stała czasowa typu J */ 

// Komendy ROM 1-Wire
#define OWIC_READ_ROM   	(0x33)	/*!< Komenda READ ROM */
#define OWIC_SKIP_ROM		(0xCC)	/*!< Komenda SKIP ROM */
#define OWIC_MATCH_ROM		(0x55)	/*!< Komenda MATCH ROM */
#define OWIC_ROM_SEARCH		(0xF0)	/*!< Komenda SEARCH ROM */

/* Sekcja deklaracji ---------------------------------------------------------*/

// --->Funkcje

// Inicjalizacja magistrali 1-Wire (impuls RESET i PRESENCE)
bool OWIMaster_Init(void);
// Odczyt kodu ROM (w przypadku kiedy jest jedno urządzenie SLAVE)
bool OWIMaster_ReadRomCode(OWIROMCode_t *romCode);
// Wysłanie kodu ROM (adresowanie urządzenia)
void OWIMaster_SendRomCode(OWIROMCode_t *romCode);
// Odbieranie bajtu po 1-Wire
uint8_t OWIMaster_ReadByte(void);
// Odczyt zadanej liczby bajtów po 1-Wire
void OWIMaster_ReadBytes(uint8_t *data, uint8_t length);
// Zapis zadanej liczby bajtów po 1-Wire
void OWIMaster_SendBytes(uint8_t *data, uint8_t length);
// Wysłanie komendy SKIP ROM po 1-Wire
void OWIMaster_SkipRom(void);
// Wysłanie bajtu po 1-Wire
void OWIMaster_SendByte(uint8_t byte);
// Wyszukiwanie urządzeń SLAVE na 1-Wire
uint8_t OWIMaster_SearchBuses(OWIDevice_t *devices, 
                              uint8_t length,
							  EOWIFamily_t *familyCodes);
// Sprawdzanie czy kod rodziny znajduje się na liście
bool OWIMaster_IsFamilyCodeExist(EOWIFamily_t *codes, EOWIFamily_t familyCode);

#endif 								/* OWI_MASTER_H_ */

/******************* (C) COPYRIGHT 2011 HENIUS *************** KONIEC PLIKU ***/
