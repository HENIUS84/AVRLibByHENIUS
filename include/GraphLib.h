/**
 *******************************************************************************
 * @file     GraphLib.h                                                      
 * @author   HENIUS (Paweł Witak)                                              
 * @version  1.01.001                                                          
 * @date     23/08/2011                                                        
 * @brief    Implementacja biblioteki graficznej (plik nagłówkowy)                                 
 *******************************************************************************
 *
 * <h2><center>COPYRIGHT 2011 HENIUS</center></h2>
 */

#ifndef  GRAPHLIB_H
#define  GRAPHLIB_H

/* Sekcja include ------------------------------------------------------------*/

// Pliki systemowe
#include <stdbool.h>
#include <stdint.h>

// Pliki użytkownika

/* Sekcja stałych, makr i definicji ------------------------------------------*/

// Konfiguracja w zależności od mikrokontrolera
#define GL_AVR						/*!< Mikrokontroler AVR */

// --->Stałe

#define MAX_WORD_LENGTH		(100)	/*!< Maksymalna długość słowa */ 

// Kody polskich znaków w Latin-2
#define P_LETTER_A 			(0xA1)	/*!< Litera Ą */
#define P_LETTER_C 			(0xC6)	/*!< Litera Ć */
#define P_LETTER_E 			(0xCA)	/*!< Litera Ę */
#define P_LETTER_L 			(0xA3)	/*!< Litera Ł */
#define P_LETTER_N 			(0xD1)	/*!< Litera Ń */	
#define P_LETTER_O 			(0xD3)	/*!< Litera Ó */
#define P_LETTER_S 			(0xA6)	/*!< Litera Ś */
#define P_LETTER_X 			(0xAC)	/*!< Litera Ź */
#define P_LETTER_Z			(0xAF)	/*!< Litera Ż */
#define P_LETTER_a			(0xB1)	/*!< Litera ą */
#define P_LETTER_c 			(0xE6)	/*!< Litera ć */
#define P_LETTER_e 			(0xEA)	/*!< Litera ę */
#define P_LETTER_l 			(0xB3)	/*!< Litera ł */
#define P_LETTER_n 			(0xF1)	/*!< Litera ń */
#define P_LETTER_o 			(0xF3)	/*!< Litera ó */
#define P_LETTER_s 			(0xB6)	/*!< Litera ś */
#define P_LETTER_x 			(0xBC)	/*!< Litera ź */
#define P_LETTER_z 			(0xBF)	/*!< Litera ż */

// --->Typy

/**
 * @brief Standardowy kolor 16-bitowy
 */
typedef enum 
{
    SC_WHITE = 0xFFFF,				/*!< Biały */
    SC_BLACK = 0					/*!< Czarny */
}StandardColor_t;

/**
 * @brief Dowolny kolor 16-bitowy (R,G,B -> 5,6,5 bitów)
 */
typedef uint16_t Color_t;

/**
 * @brief Struktura opisująca punkt
 */
typedef struct 
{
    int16_t X;						/*!< Wpółrzędna X */
    int16_t Y;						/*!< Wpółrzędna Y */
}Point_t;

/**
 * @brief Struktura opisująca rozmiar
 */
typedef struct 
{
    uint16_t Width;					/*!< Szerokość */
    uint16_t Height;				/*!< Wysokość */
}Size_t;

/**
 * @brief Nagłówek obrazka
 */
typedef struct 
{
    uint8_t ColorPalette;			/*!< Paleta kolorów (w bitach) */
    Size_t Size;					/*!< Rozmiar obrazka */
}BitmapHeader_t;

/**
 * @brief Rodzaje wyrównania pionowego
 */
typedef enum 
{
	VA_TOP,							/*!< Góra */
	VA_CENTER,						/*!< Środek */
	VA_BOTTOM						/*!< Dół */
}VerticalAlignment_t;

/**
 * @brief Rodzaje wyrównania poziomego
 */
typedef enum 
{
	HA_LEFT,						/*!< Lewa strona */
	HA_CENTER,						/*!< Środek */
	HA_RIGHT						/*!< Prawa strona */
}HorizontalAlignment_t;

/**
 * @brief Opis struktury grubości
 */
typedef struct 
{
    uint8_t Top;					/*!< Górna grubość */
    uint8_t Bottom;					/*!< Dolna grubość */
    uint8_t Left;					/*!< Lewa grubość */
    uint8_t Right;					/*!< Prawa grubość */
}Thickness_t;

/**
 * @brief Składowe RGB koloru
 */
typedef struct 
{
	uint8_t Red;					/*!< Składowa czerwona */
	uint8_t Green;					/*!< Składowa zielona */
	uint8_t Blue;					/*!< Składowa niebieska */
}RGB_t;

/**
 * @brief Opis czcionki
 */
typedef struct 
{
    uint8_t WordSize;				/*!< Rozmiar słowa danych w bitach */
    uint8_t Height;					/*!< Wysokość kanwy znaku */ 
	uint8_t *Index;					/*!< Tabela indeksów znaków */   
    uint16_t *Offset;				/*!< Tabela offsetów w tabeli danych */
    uint8_t *Width;					/*!< Tabela rozmiarów kanwy znaków */	    
	uint8_t *Data;					/*!< Tabela danych */	
}Font_t;

/**
 * @brief Opis komponentu TextBlock
 */
typedef struct 
{
	Size_t Size;					/*!< Rozmiar komponentu */
	Point_t Location;				/*!< Lokalizacja lewego górnego rogu */	
	Point_t TextOffset;				/*!< Przesunięcie tekstu */
	Color_t TextColor;				/*!< Kolor tekstu */
	Color_t BackgroundColor;		/*!< Kolor tła */
	bool IsTransparentBackground;	/*!< Flaga oznaczająca przezroczyste tło */
	Color_t BorderColor;			/*!< Kolor ramki otaczającej */
	Thickness_t BorderThickness;	/*!< Grubość ramki */
	HorizontalAlignment_t HorizontalAlignment;	/*!< Wyrównanie poziome */
	VerticalAlignment_t VerticalAlignment;		/*!< Wyrównanie pionowe */
	uint8_t CharSpacing;			/*!< Odstęp pomiędzy znakami */
	Font_t *Font;					/*!< Typ czcionki */
}TextBlock_t;

/**
 * @brief Opis okręgu
 */
typedef struct 
{
	Point_t Location;				/*!< Lokalizacja na ekranie */
	uint8_t Radius;					/*!< Promień */
	Color_t Color;					/*!< Kolor linii */
}Circle_t;

/**
 * @brief Opis lini
 */
typedef struct 
{
	Point_t From;					/*!< Punkt początkowy */
	Point_t To;						/*!< Punkt końcowy */
	Color_t Color;					/*!< Kolor linii */
}Line_t;

/**
 * @brief Opis prostokąta
 */
typedef struct 
{
	Point_t Location;				/*!< Loaklizacja (lewy górny róg) */
	Size_t Size;					/*!< Rozmiar */
	Color_t LineColor;				/*!< Kolor linii */
	Color_t BackgroundColor;		/*!< Kolor wypełnienia */
	bool IsTransparentBackground;	/*!< Flaga oznaczająca przezroczyste tło */
}Rectangle_t;

/**
 * @brief Opis paska postępu
 */
typedef struct 
{
	Point_t Location;				/*!< Lokalizacja paska */
	Size_t Size;					/*!< Rozmiar (bez ramki) */
	uint8_t BorderThickness;		/*!< Grubość ramki */
	Color_t BorderColor;			/*!< Kolor ramki otaczającej */
	Color_t BackgroundColor;		/*!< Kolor tła */
	Color_t StartColor;				/*!< Startowy kolor gradientu poziomego */
	Color_t EndColor;				/*!< Końcowy kolor gradientu poziomego */
	Color_t CentralColor;			/*!< Centralny kolor gradientu pionowego */
	uint16_t Minimum;				/*!< Minimalna wartość */
	uint16_t Maximum;				/*!< Maksymalna wartość */
	bool IsTransparentBackground;	/*!< Flaga oznaczająca przezroczyste tło */
}ProgressBar_t;

/* Sekcja deklaracji ---------------------------------------------------------*/

// Inicjalizacja biblioteki graficznej
void InitGraphLib(void(*setPixel)(uint16_t x, uint16_t y, Color_t pixelColor));
// Konwersja koloru 16 bitowego na składowe RGB
RGB_t ConvertColorToRGB(Color_t color);
// Konwersja składowych RGB na kolor
Color_t ConvertRGBToColor(RGB_t rgb);
// Interpolacja dwukolorowa
Color_t Interpolate2Colors(Color_t colorA, Color_t colorB, 
                           uint16_t pos, uint16_t max);
// Interpolacja trójkolorowa
Color_t Interpolate3Colors(Color_t colorA, Color_t colorB, Color_t colorC, 
                           uint16_t pos, uint16_t max);
// Rysowanie komponentu (z pamięci RAM) TextBlock 
void PutTextBlock(TextBlock_t *textBlock, uint8_t *text);
// Rysowanie komponentu (z pamięci Flash) TextBlock
void PutTextBlock_P(const TextBlock_t *textBlock, uint8_t *text);
// Rysowanie okręgu (z pamięci RAM)
void PutCircle(Circle_t *circle);
// Rysowanie okręgu (z pamięci Flash)
void PutCircle(Circle_t *circle);
// Rysowanie linii (z pamięci RAM)
void PutLine(Line_t *line);
// Rysowanie linii (z pamięci Flash)
void PutLine_P(Line_t *line);
// Rysowanie prostokąta (z pamięci RAM)
void PutRectangle(Rectangle_t *rectangle);
// Rysowanie prostokąta (z pamięci Flash)
void PutRectangle_P(Rectangle_t *rectangle);
// Rysowanie paska postępu (z pamięci RAM)
void PutProgressBar(ProgressBar_t *progressBar, uint16_t val);	
// Rysowanie paska postępu (z pamięci Flash)
void PutProgressBar_P(ProgressBar_t *progressBar, uint16_t val);	
// Umieszczanie na ekranie obrazka (z pamięci RAM)
void PutImage(uint8_t *image, int16_t x, int16_t y);
// Umieszczanie na ekranie obrazka (z pamięci Flash)
void PutImage_P(uint8_t *image, int16_t x, int16_t y);			   

#endif										/* GRAPHLIB_H */

/******************* (C) COPYRIGHT 2011 HENIUS *************** KONIEC PLIKU ***/
