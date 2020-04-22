/**
 *******************************************************************************
 * @file     KS0108.h
 * @author   HENIUS (Paweł Witak)
 * @version  1.01.002
 * @date     14/03/2013
 * @brief    Obsługa wyświetlacza graficznego opartego na KS0108 (plik
 *           nagłówkowy)
 *******************************************************************************
 *
 * <h2><center>COPYRIGHT 2013 HENIUS</center></h2>
 */

#ifndef KS0108LCD_H_
#define KS0108LCD_H_

/* Sekcja include ------------------------------------------------------------*/

// Pliki systemowe
#include <avr/io.h>
#include <avr/delay.h>
#include <stdbool.h>

/* Sekcja stałych, makr i definicji ------------------------------------------*/

/*! Aktywacja bufora ramek */
#define KS0108LCD_FRAME_BUFF_ENABLED

// --->Definicje

// Port szyny danych
#define KS0108LCD_DATA_PORT	(PORT?) /*!< Rejestr wyjściowy szyny danych */
#define KS0108LCD_DATA_PIN	(PIN?)  /*!< Rejestr wejściowy szyny danych */
#define KS0108LCD_DATA_DDR  (DDR?)  /*!< Rejestr kierunkowy szyny danych */

// Port sygnału RS
#define KS0108LCD_RS_PORT   (PORT?) /*!< Rejestr wyjściowy sygnału RS */
#define KS0108LCD_RS_PIN    (PIN?)	/*!< Rejestr wejściowy sygnału RS */
#define KS0108LCD_RS_DDR    (DDR?)	/*!< Rejestr kierunkowy sygnału RS */
#define KS0108LCD_RS        (_BV(?))/*!< Maska pinu sygnału RS */

// Port sygnału RW
#define KS0108LCD_RW_PORT   (PORT?)	/*!< Rejestr wyjściowy sygnału RW */
#define KS0108LCD_RW_PIN    (PIN?)	/*!< Rejestr wyjściowy sygnału RW */
#define KS0108LCD_RW_DDR    (DDR?)	/*!< Rejestr kierunkowy sygnału RW */
#define KS0108LCD_RW        (_BV(?))/*!< Maska pinu sygnału RW */

// Port sygnału EN
#define KS0108LCD_EN_PORT   (PORT?) /*!< Rejestr wyjściowy sygnału EN */
#define KS0108LCD_EN_PIN    (PIN?)	/*!< Rejestr wejściowy sygnału EN */
#define KS0108LCD_EN_DDR    (DDR?)	/*!< Rejestr kierunkowy sygnału EN */
#define KS0108LCD_EN		(_BV(?))/*!< Maska pinu sygnału EN */

// Port sygnału RST
#define KS0108LCD_RST_PORT  (PORT?)	/*!< Rejestr wyjściowy sygnału RST */
#define KS0108LCD_RST_PIN   (PIN?)	/*!< Rejestr wejściowy sygnału RST */
#define KS0108LCD_RST_DDR   (DDR?)	/*!< Rejestr kierunkowy sygnału RST */
#define KS0108LCD_RST       (_BV(?))/*!< Maska pinu sygnału RST */

// Port sygnału CS1
#define KS0108LCD_CS1_PORT	(PORT?)	/*!< Rejestr wyjściowy sygnału CS1 */
#define KS0108LCD_CS1_PIN	(PIN?)	/*!< Rejestr wejściowy sygnału CS1 */
#define KS0108LCD_CS1_DDR	(DDR?)	/*!< Rejestr kierunkowy sygnału CS1 */
#define KS0108LCD_CS1		(_BV(?))/*!< Maska pinu sygnału CS1 */

// Port sygnału CS2
#define KS0108LCD_CS2_PORT	(PORT?)	/*!< Rejestr wyjściowy sygnału CS2 */
#define KS0108LCD_CS2_PIN	(PIN?)	/*!< Rejestr wejściowy sygnału CS2 */
#define KS0108LCD_CS2_DDR	(DDR?)	/*!< Rejestr kierunkowy sygnału CS2 */
#define KS0108LCD_CS2		(_BV(?))/*!< Maska pinu sygnału CS2 */

// Port sygnału CS3
//#define KS0108LCD_CS3_PORT	(PORT?)		/*!< Rejestr wyjściowy sygnału CS3 */
#ifdef KS0108LCD_CS3_PORT
#define KS0108LCD_CS3_PIN	(PIN?)	/*!< Rejestr wejściowy sygnału CS3 */
#define KS0108LCD_CS3_DDR   (DDR?)	/*!< Rejestr kierunkowy sygnału CS3 */
#define KS0108LCD_CS3       (_BV(?))/*!< Maska pinu sygnału CS3 */
#endif	

/*! Sygnał strobujący na linii EN */
#define KS0108LCD_STROBE()    \
	KS0108LCD_EN_PORT |= KS0108LCD_EN; \
	KS0108LCD_EN_PORT &= ~KS0108LCD_EN
 
// Definicja flag									                    
#define KS0108LCD_BUSY			(0x80)	/*!< Flaga zajętości kontrolera */
#define KS0108LCD_SET_Y			(0xB8)	/*!< Flaga wyboru współ. y */
#define KS0108LCD_SET_X			(0x40)	/*!< Flaga wyboru współ. x */
#define KS0108LCD_START_LINE	(0xC0)  /*!< Flaga ustawiania lini startowej */
#define KS0108LCD_STATE			(0x3E)  /*!< Flaga stanu kontrolera */
#define KS0108LCD_ON			(0x01)  /*!< Flaga włączania kontrolera */
#define KS0108LCD_OFF			(0x00)	/*!< Flaga wyłączania kontrolera */

// Definicja stałych
/*! Rozmiar pojedynczego kontrolera (w pikselach) */
#define KS0108_SIZE			(64)	
#ifdef KS0108LCD_CS3_PORT
/*! Szerokość wyświetlacza */
#define KS0108LCD_WIDTH     (KS0108_SIZE * 3)	
#elif defined(KS0108LCD_CS2_PORT) 
/*! Szerokość wyświetlacza */
#define KS0108LCD_WIDTH     (KS0108_SIZE * 2)  
#else
/*! Szerokość wyświetlacza */
#define KS0108LCD_WIDTH     (KS0108_SIZE)	
#endif								// KS0108LCD_CS3_PORT
/*! Wysokość wyświetlacza */
#define KS0108LCD_HEIGHT    (KS0108_SIZE)    
/*! Kolor biały (zapalenie piksela) */
#define KS0108LCD_WHITE_COLOR	(0xFFFF)
/*! Kolor czarny (zgaszenie piksela) */
#define KS0108LCD_BLACK_COLOR   (0x0000)

/* Sekcja deklaracji ---------------------------------------------------------*/

// Inicjalizacja sterownika KS0108 
void KS0108LCD_Init(void);     
// Włączanie wyświetlacza        
void KS0108LCD_On(void);               
// Wyłączanie wyświetlacza
void KS0108LCD_Off(void);              
// Skok kursora pod wskazaną pozycję
void KS0108LCD_GoTo(uint16_t x, uint16_t y);		
// Odczyt danych z wyświetlacza
uint8_t KS0108LCD_ReadData(void);		
// Ustawianie całej pamięci wyświetlacza
void KS0108LCD_MemoryHandler(void);        
// Zapełnianie wyświetlacza zadanym wzorcem
void KS0108LCD_Clear(uint8_t pattern); 
// Zapalanie piksela zadanym kolorem
void KS0108LCD_SetPixel(uint16_t x, uint16_t y, uint16_t color);
// Wyświetlanie obrazu (z pamięci Flash)
void KS0108LCD_PutImage_P(uint8_t *image, 
                          int16_t offsetX, int16_t offsetY, 
					      bool isTransparentBackground);
// Blokowanie wyświetlania obrazu (tylko w trybie FRAME BUFFER)
void KS0108LCD_BlockScreen(bool isBlocked);

#endif								/* KS0108LCD_H_ */

/******************* (C) COPYRIGHT 2013 HENIUS *************** KONIEC PLIKU ***/