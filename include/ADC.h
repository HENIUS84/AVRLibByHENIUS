/**
 *******************************************************************************
 * @file     ADC.h
 * @author   HENIUS (Paweł Witak)
 * @version  1.01.001
 * @date     08-05-2011
 * @brief    Obsługa przetwornika A/C (plik nagłówkowy)
 *******************************************************************************
 *
 * <h2><center>COPYRIGHT 2011 HENIUS</center></h2>
 */

#ifndef  ADC_H_
#define  ADC_H_

/* Sekcja include ------------------------------------------------------------*/

// Pliki systemowe
#include <avr/io.h>

// Pliki użytkownika

/* Sekcja stałych, makr i definicji ------------------------------------------*/

// Stałe
#define ADC_CHANNEL_MASK	(~0x1F)	/*!< Maska bitowa dla wyboru kanału ADC */

// Makra

/* Sekcja deklaracji ---------------------------------------------------------*/

// --->Zmienne

// --->Typy
/**
 * @brief Tryb pomiaru
 */
typedef enum
{
	ADCM_SINGLE,					/*!< Pojedynczy pomiar */
	ADCM_AUTO        				/*!< Ciągły pomiar */
}ADCmode_t;

/**
 * @brief Rozdzieloczość próbki
 */
typedef enum
{
	ADCR_10BIT = 0,					/*!< Rozdzielczość 8-bitowa */
	ADCR_8BIT  = _BV(ADLAR),		/*!< Rozdzielczość 10-bitowa */
	ADCR_11BIT = 1,					/*!< Rozdzielczość 11-bitowa 
	                                     (oversampling) */
	ADCR_12BIT = 2,					/*!< Rozdzielczość 11-bitowa 
	                                     (oversampling) */
}ADCresolution_t;

/**
 * @brief Szybkość próbkowania
 */
typedef enum
{
#if F_CPU >= 8000000
	ADCSR_77KSPS   = 3,				/*!< 77k próbek/s */
	ADCSR_39KSPS   = 4,             /*!< 39k próbek/s */
	ADCSR_19200SPS = 5,             /*!< 19200 próbek/s */
	ADCSR_9600SPS  = 6,             /*!< 9600 próbek/s */
	ADCSR_4800SPS  = 7              /*!< 4800 próbek/s */
#else
	ADCSR_77KSPS   = 2,
	ADCSR_39KSPS   = 2,
	ADCSR_19200SPS = 4,
	ADCSR_9600SPS  = 5,
	ADCSR_4800SPS  = 6
#endif
}ADCsampleRate_t;

/**
 * @brief Typ napięcia referencyjnego
 */
typedef enum
{
	ADCVR_AREF = (0 << REFS0),		/*!< Napięcie referencyjne  AREF*/
	ADCVR_AVCC = (1 << REFS0),		/*!< Napięcie zasilania */
	ADCVR_INT  = (3 << REFS0)		/*!< Wewnętrzne napięcie referencyjne
	                                     2,56 V */
}ADCvref_t;

/**
 * @brief Kanały przetwornika A/C
 */
typedef enum
{
	// Kanały pojedyncze
	ADCC_CH0          = 0x00,		/*!< Kanał 0 */
	ADCC_CH1          = 0x01,		/*!< Kanał 1 */
	ADCC_CH2          = 0x02,		/*!< Kanał 2 */
	ADCC_CH3          = 0x03,		/*!< Kanał 3 */
	ADCC_CH4          = 0x04,		/*!< Kanał 4 */
	ADCC_CH5          = 0x05,		/*!< Kanał 5 */
	ADCC_CH6          = 0x06,		/*!< Kanał 6 */
	ADCC_CH7          = 0x07,		/*!< Kanał 7 */

	// Kanały różnicowe
	// Wzmocnienie x1
	ADCC_CH0_CH1_X1   = 0x10,		/*!< Kanał 0 +, kanał 1 - */
	ADCC_CH1_CH1_X1   = 0x11,		/*!< Kanał 1 +, kanał 1 - */
	ADCC_CH2_CH1_X1   = 0x12,		/*!< Kanał 2 +, kanał 1 - */
	ADCC_CH3_CH1_X1   = 0x13,		/*!< Kanał 3 +, kanał 1 - */
	ADCC_CH4_CH1_X1   = 0x14,		/*!< Kanał 4 +, kanał 1 - */
	ADCC_CH5_CH1_X1   = 0x15,		/*!< Kanał 5 +, kanał 1 - */
	ADCC_CH6_CH1_X1   = 0x16,		/*!< Kanał 6 +, kanał 1 - */
	ADCC_CH7_CH1_X1   = 0x17,		/*!< Kanał 7 +, kanał 1 - */
	ADCC_CH0_CH2_X1   = 0x18,		/*!< Kanał 0 +, kanał 2 - */
	ADCC_CH1_CH2_X1   = 0x19,		/*!< Kanał 1 +, kanał 2 - */
	ADCC_CH2_CH2_X1   = 0x1A,		/*!< Kanał 2 +, kanał 2 - */
	ADCC_CH3_CH2_X1   = 0x1B,		/*!< Kanał 3 +, kanał 2 - */
	ADCC_CH4_CH2_X1   = 0x1C,		/*!< Kanał 4 +, kanał 2 - */
	ADCC_CH5_CH2_X1   = 0x1D,		/*!< Kanał 5 +, kanał 2 - */
	// Wzmocnienie x10
	ADCC_CH0_CH0_X10  = 0x08,		/*!< Kanał 0 +, kanał 0 - */
	ADCC_CH1_CH0_X10  = 0x09,		/*!< Kanał 1 +, kanał 0 - */
	ADCC_CH2_CH2_X10  = 0x0C,		/*!< Kanał 2 +, kanał 2 - */
	ADCC_CH3_CH2_X10  = 0x0D,		/*!< Kanał 3 +, kanał 2 - */
	// Wzmocnienie x200
	ADCC_CH0_CH0_X200 = 0x0A,		/*!< Kanał 0 +, kanał 0 - */
	ADCC_CH1_CH0_X200 = 0x0B,		/*!< Kanał 1 +, kanał 0 - */
	ADCC_CH2_CH2_X200 = 0x0E,		/*!< Kanał 2 +, kanał 2 - */
	ADCC_CH3_CH2_X200 = 0x0F,		/*!< Kanał 3 +, kanał 2 - */

	// Pozostałe kanały
	ADCC_1_22V		  = 0x1E,		/*!< Pomiar napięcia 1,22 V */
	ADCC_GND		  = 0x1F,		/*!< Pomiar napięcia 0 V */
}ADCChannel_t;

/**
 * @brief Struktura opisująca przetwornik A/C
 */
typedef struct
{
	ADCmode_t Mode;					/*!< Tryb pomiaru */
	ADCsampleRate_t SampleRate;		/*!< Szybkość próbkowania */
	ADCresolution_t Resolution;		/*!< Rozdzielczość pomiaru */
	ADCvref_t Vref;					/*!< Napięcie referencyjne */
	ADCChannel_t *ChannelList;		/*!< Lista mierzonych kanałów */
	uint8_t ChannelAmount;			/*!< Liczba mierzonych kanałów */
	volatile uint16_t *Buffer;		/*!< Bufor na dane pomiarowe z kanałów */
	volatile uint32_t *OVSbuffer;	/*!< Bufor dla celów oversamplingu */
	void (*OnCompleted)();			/*!< Callback na zakończenie pomiaru
	                                     wszystkich kanałów z listy */
}ADC_t;

// --->Funkcje
// Inicjalizacja przetwornika A/C
void ADC_Init(ADC_t *adcConfig);
// Rozpoczęcie pomiaru
void ADC_StartConv(void);

#endif								/* ADC_H_ */

/******************* (C) COPYRIGHT 2011 HENIUS ************** KONIEC PLIKU ****/
