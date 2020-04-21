/**
 *******************************************************************************
 * @file     KS0108.c
 * @author   HENIUS (Paweł Witak)
 * @version  1.1.3
 * @date     12/04/2014
 * @brief    Obsługa wyświetlacza graficznego opartego na KS0108
 *******************************************************************************
 *
 * <h2><center>COPYRIGHT 2014 HENIUS</center></h2>
 */

/* Sekcja include ------------------------------------------------------------*/

// --->Pliki systemowe

#include <stdint.h>
#include <avr/pgmspace.h>

// --->Pliki użytkownika

#include "KS0108LCD.h"

/* Sekcja zmiennych ----------------------------------------------------------*/

uint8_t LCD_x = 0;					/*!< Współrzędne globalne X */
uint8_t LCD_y = 0;               	/*!< Współrzędne globalne Y */

#ifdef KS0108LCD_FRAME_BUFF_ENABLED 

/*! Flaga oznaczająca blokowanie obrazu */
bool IsScreenBlocked = false;
/*! Bufor ramek wyświetlacza */
uint8_t FrameBuffer[KS0108LCD_WIDTH][KS0108LCD_HEIGHT / 8];

#endif

/* Sekcja funkcji ------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/**
 * @brief    Wybór kontrolera
 * @param    number : numer kontrolera
 * @retval   Brak
 */
void KS0108LCD_SelectController(uint8_t number)
{
	switch (number)
	{
		// --->Kontroler #1
		case 1:
			KS0108LCD_CS1_PORT |= KS0108LCD_CS1;
			KS0108LCD_CS2_PORT &= ~KS0108LCD_CS2;
			#ifdef KS0108LCD_CS3_PORT
			KS0108LCD_CS3_PORT &= ~KS0108LCD_CS3;
			#endif					// KS0108LCD_CS3_PORT
		
			break;
			
		// --->Kontroler #2
		case 2:
			KS0108LCD_CS1_PORT &= ~KS0108LCD_CS1;
			KS0108LCD_CS2_PORT |= KS0108LCD_CS2;
			#ifdef KS0108LCD_CS3_PORT
			KS0108LCD_CS3_PORT &= ~KS0108LCD_CS3;
			#endif					// KS0108LCD_CS3_PORT
		
			break;
		
		// --->Kontroler #3
		case 3:
			KS0108LCD_CS1_PORT &= ~KS0108LCD_CS1;
			KS0108LCD_CS2_PORT &= ~KS0108LCD_CS2;
			#ifdef KS0108LCD_CS3_PORT
			KS0108LCD_CS3_PORT |= KS0108LCD_CS3;
			#endif					// KS0108LCD_CS3_PORT
		
			break;
	}
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Wyłączenie kontrolerów
 * @param    Brak
 * @retval   Brak
 */
void KS0108LCD_NoCS(void)
{
	KS0108LCD_CS1_PORT &= ~KS0108LCD_CS1;
	#ifdef KS0108LCD_CS2_PORT
	KS0108LCD_CS2_PORT &= ~KS0108LCD_CS2;
	#endif							// KS0108LCD_CS2_PORT
	#ifdef KS0108LCD_CS3_PORT
	KS0108LCD_CS3_PORT &= ~KS0108LCD_CS3;
	#endif							// KS0108LCD_CS3_PORT
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Inicjalizacja sterownika KS0108
 * @param    Brak
 * @retval   Brak
 */
void KS0108LCD_Init(void)
{
	// Inicjalizacja portów
	KS0108LCD_DATA_DDR = 0xFF;
	KS0108LCD_RS_DDR |= KS0108LCD_RS;
	KS0108LCD_RW_DDR |= KS0108LCD_RW;
	KS0108LCD_EN_DDR |= KS0108LCD_EN;
	KS0108LCD_RST_DDR |= KS0108LCD_RST;
	KS0108LCD_CS1_DDR |= KS0108LCD_CS1;
	KS0108LCD_CS2_DDR |= KS0108LCD_CS2;

	KS0108LCD_CS1_PORT |= KS0108LCD_CS1;
	KS0108LCD_CS2_PORT |= KS0108LCD_CS2;
	KS0108LCD_EN_PORT |= KS0108LCD_EN;
	KS0108LCD_RW_PORT |= KS0108LCD_RW;
	KS0108LCD_RS_PORT |= KS0108LCD_RS;
	KS0108LCD_RST_PORT |= KS0108LCD_RST;
   
	KS0108LCD_On();					// Włączanie wyświetlacza
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Czekanie na zakończenie obsługi poprzedniej instrukcji
 * @param    Brak
 * @retval   Brak
 */
void KS0108LCD_Wait(void)
{
	uint16_t timeoutTimer = 10;	// Timer timeout'u
	
	// Będziemy czytać dane z wyświetlacza
	KS0108LCD_DATA_DDR = 0x00;             
	KS0108LCD_RS_PORT &= ~KS0108LCD_RS;          
	KS0108LCD_RW_PORT |= KS0108LCD_RW;            
	
	// Sprawdzanie flagi BUSY                                            
	do                                            
	{
		KS0108LCD_EN_PORT |= KS0108LCD_EN;  
		KS0108LCD_EN_PORT &= ~KS0108LCD_EN;
	}while ((KS0108LCD_DATA_PIN & KS0108LCD_BUSY) && --timeoutTimer);        
                                                	
	KS0108LCD_DATA_DDR = 0xFF;         // Z powrotem port jako wyjście
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Zapis instrukcji do wyświetlacza
 * @param    cmd : Instrukcja dla sterownika
 * @retval   Brak
 */
void KS0108LCD_WriteCmd(uint8_t cmd)
{
	KS0108LCD_Wait();
	KS0108LCD_RS_PORT &= ~KS0108LCD_RS;	
	KS0108LCD_RW_PORT &= ~KS0108LCD_RW; 
	KS0108LCD_DATA_DDR |= 0xFF; 
	KS0108LCD_DATA_PORT = cmd;      // Zapisujemy instrukcję
	KS0108LCD_STROBE();				// Sygnał strobujący
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Zapis danych do wyświetlacza
 * @param    data : Bajt dla sterownika
 * @retval   Brak
 */
void KS0108LCD_WriteData(uint8_t data)
{  
	// Wybór kontrolera
	if (LCD_x < KS0108_SIZE)
	{
		KS0108LCD_SelectController(1);
	}
	else if (LCD_x >= KS0108_SIZE && LCD_x < (KS0108_SIZE * 2))
	{
		KS0108LCD_SelectController(2);
	}
	else
	{
		KS0108LCD_SelectController(3);
	}
	
	KS0108LCD_Wait();               // Oczekiwanie na gotowość kontrolera
	KS0108LCD_RS_PORT |= KS0108LCD_RS;    // Wysoki stan na linii RS -> dane  
	KS0108LCD_RW_PORT &= ~KS0108LCD_RW;   // Niski stan na linii RW -> zapis
	KS0108LCD_DATA_PORT = data;     // Wystawienie na port danej  
	KS0108LCD_STROBE();             // Sygnał strobujścy
	LCD_x++;                        // Zwiększenie współrzędnej x wyświetlacza
									// (pomocniczej)
	if (LCD_x > KS0108LCD_WIDTH)	// Jeśli koniec ekranu
	{
		LCD_x = 0;                  // to wyzeruj współrzędną x
	}
	KS0108LCD_NoCS();
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Odczyt danych z wyświetlacza
 * @param    Brak
 * @retval : Odczytany bajt
 */
uint8_t KS0108LCD_ReadData(void)
{
	uint8_t data = 0;               // Odczytana dana

	// Wybór kontrolera
	if (LCD_x < KS0108_SIZE)
	{
		KS0108LCD_SelectController(1);
	}
	else if (LCD_x >= KS0108_SIZE && LCD_x < (KS0108_SIZE * 2))
	{
		KS0108LCD_SelectController(2);
	}
	else
	{
		KS0108LCD_SelectController(3);
	}
	
	KS0108LCD_Wait();					// Oczekiwanie na gotowość kontrolera
	KS0108LCD_RS_PORT |= KS0108LCD_RS;	// Wysoki stan na linii RS -> dane   
	KS0108LCD_RW_PORT |= KS0108LCD_RW;	// Wysoki stan na linii RW -> odczyt  
	KS0108LCD_EN_PORT |= KS0108LCD_EN;  // Wysoki stan na linii EN  
	KS0108LCD_EN_PORT |= KS0108LCD_EN; 
	asm("nop");
	KS0108LCD_DATA_DDR = 0x00;      // Port danych jako wejście
	KS0108LCD_DATA_PORT = 0xFF;		// Podciąganie końcówek portu
	data = KS0108LCD_DATA_PIN;      // Odczyt danych z portu
	KS0108LCD_EN_PORT &= ~KS0108LCD_EN; // Niski stan na linii EN
	LCD_x++;                        // Zwiększenie współrzędnej x wyświetlacza
	
	// Jeśli koniec ekranu
	if (LCD_x > (KS0108LCD_WIDTH - 1))   
	{
		LCD_x = 0;                  // to wyzeruj współrzędną x
	}
	KS0108LCD_NoCS();
	KS0108LCD_DATA_DDR = 0xFF;      // Port danych z powrotem jako wyjście

	return data;
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Skok kursora pod wskazaną pozycję
 * @param    x : wpółrzęna pozioma
 * @param    y : współrzędna pionowa
 * @retval   Brak
 */
void KS0108LCD_GoTo(uint16_t x, uint16_t y)
{  
	// Podajemy adresy, więc RS i RW -> 0
	KS0108LCD_RS_PORT &= ~KS0108LCD_RS;	
	KS0108LCD_RW_PORT &= ~KS0108LCD_RW;	
   
	LCD_x = x;                      // Przypisanie współrzędnym globalnym nowych
	LCD_y = y;                      // wartości
   
	// Wybór kontrolera w zależności od współrzędnej poziomej
	if (LCD_x >= KS0108_SIZE && LCD_x < (KS0108_SIZE * 2)) 
	{
		KS0108LCD_SelectController(2);  
		            
		// Zapis współrzednej pionowej
		KS0108LCD_WriteCmd(KS0108LCD_SET_Y | LCD_y);      
		// Zapis wspórzędnej poziomej
		KS0108LCD_WriteCmd(KS0108LCD_SET_X | (LCD_x - KS0108_SIZE));  
	}
	else if (LCD_x >= (KS0108_SIZE * 2))
	{
		KS0108LCD_SelectController(3);
		
		// Zapis współrzednej pionowej
		KS0108LCD_WriteCmd(KS0108LCD_SET_Y | LCD_y);
		// Zapis wspórzędnej poziomej
		KS0108LCD_WriteCmd(KS0108LCD_SET_X | (LCD_x - (KS0108_SIZE * 2)));
	}
	else                            
	{
		// Uaktywnienie pierwszego kontrolera
		KS0108LCD_SelectController(1);              
		// Zapis współrzędnej pionowej
		KS0108LCD_WriteCmd(KS0108LCD_SET_Y | LCD_y);
		// Zapis współrzędnej poziomej
		KS0108LCD_WriteCmd(KS0108LCD_SET_X | LCD_x);  
		// Uaktywnienie drugiego kontrolera
		KS0108LCD_SelectController(2);                          
		// Zapis współrzędnej pionowej
		KS0108LCD_WriteCmd(KS0108LCD_SET_Y | LCD_y);  
		// Wyzerowanie współrzędnej poziomej
		KS0108LCD_WriteCmd(KS0108LCD_SET_X | 0 );     
	}
   
	// Uaktywnienie pierwszego kontrolera
	KS0108LCD_SelectController(1);                
	// Ustawienie lini początkowej na zerową
	KS0108LCD_WriteCmd(KS0108LCD_START_LINE | 0);    
	// Uaktywnienie drugiego kontrolera
	KS0108LCD_SelectController(2);                           
	// Ustawienie lini początkowej na zerową
	KS0108LCD_WriteCmd(KS0108LCD_START_LINE | 0);
	KS0108LCD_NoCS();
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Wpółczanie wyświetlacza
 * @param    Brak
 * @retval   Brak
 */
void KS0108LCD_On(void)
{
	KS0108LCD_SelectController(1);  // Aktywny pierwszy kontroler
	// Włączenie wyswietlania danych
	KS0108LCD_WriteCmd(KS0108LCD_STATE | KS0108LCD_ON); 
	KS0108LCD_SelectController(2);  // Aktywny drugi kontroler
	// Włączenie wyświetlania danych
	KS0108LCD_WriteCmd(KS0108LCD_STATE | KS0108LCD_ON); 
	KS0108LCD_SelectController(3);  // Aktywny trzeci kontroler
	// Włączenie wyświetlania danych
	KS0108LCD_WriteCmd(KS0108LCD_STATE | KS0108LCD_ON);
	KS0108LCD_NoCS();
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Wyłączanie wyświetlacza
 * @param    Brak
 * @retval   Brak
 */
void KS0108LCD_Off(void)
{
   KS0108LCD_SelectController(1);   // Aktywny pierwszy kontroler
   // Wyłączenie wyswietlania danych
   KS0108LCD_WriteCmd(KS0108LCD_STATE | KS0108LCD_OFF); 
   KS0108LCD_SelectController(2);   // Aktywny drugi kontroler
   // Wyłączenie wyświetlania danych
   KS0108LCD_WriteCmd(KS0108LCD_STATE | KS0108LCD_OFF);
   KS0108LCD_SelectController(3);   // Aktywny trzeci kontroler
   // Wyłączenie wyświetlania danych
   KS0108LCD_WriteCmd(KS0108LCD_STATE | KS0108LCD_OFF);
   KS0108LCD_NoCS();
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Blokowanie wyświetlania obrazu (tylko w trybie FRAME BUFFER)
 * @param    isBlocked : flaga oznaczająca zablokowanie obrazu
 * @retval   Brak
 */
void KS0108LCD_BlockScreen(bool isBlocked) 
{
	#ifdef KS0108LCD_FRAME_BUFF_ENABLED
	
	IsScreenBlocked = isBlocked;
	
	#endif
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Zapełnianie wyświetlacza zadanym wzorcem
 * @param    pattern : wzorzec do wyświetlenia
 * @retval   Brak
 */
void KS0108LCD_Clear(uint8_t pattern)
{
	uint16_t x, y;                  // Pomocnicze zmienne
	
	#ifdef KS0108LCD_FRAME_BUFF_ENABLED
	
	for (y = 0; y < (KS0108LCD_HEIGHT / 8); y++)          
	{
		for (x = 0; x < KS0108LCD_WIDTH; x++) 
		{   
			// Zapis do bufora ramek       
			FrameBuffer[x][y] = pattern; 
		}
	}
	
	#else	  
	for (y = 0; y < (KS0108LCD_HEIGHT / 8); y++)          
	{
		KS0108LCD_GoTo(0, y);       // Ustawienie współrzędnej
									// y wyświetlacza
		for (x = 0; x < KS0108LCD_WIDTH; x++) 
		{  
			// Zapis do wyświetlacza        
			KS0108LCD_WriteData(pattern); 
		}
	}
	KS0108LCD_GoTo(0, 0);           // Ustawienie początkowych
									// współrzędnych
	#endif
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Ustawianie całej pamięci wyświetlacza (tryb Frame Buffer)
 * @param    Brak
 * @retval   Brak
 */
void KS0108LCD_MemoryHandler(void)
{
#ifdef KS0108LCD_FRAME_BUFF_ENABLED
	static uint16_t y = 0;			// Pomocnicza zmienna y-owa
	uint16_t x;						// Pomocnicza zmienna x-owa
	
	if (!IsScreenBlocked)
	{		
		KS0108LCD_GoTo(0, y);		// Ustawienie współrzędnej
									// y wyświetlacza
		for (x = 0; x < KS0108LCD_WIDTH; x++) 
		{   
			// Zapis do wyświetlacza       
			KS0108LCD_WriteData(FrameBuffer[x][y]);
		}
	
		// Inkrementacja współrzędnej y		
		y++;
		y %= KS0108LCD_HEIGHT / 8;
	}
#endif	
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Wyświetlanie obrazu (z pamięci RAM)
 * @param    *image : kod obrazka
 * @param    offsetX : numer kolumny prawego górnego rogu obrazka
 * @param    offsetY : numer strony prawego górnego rogu obrazka
 * @param    isTransparentBackground : flaga oznaczająca przezroczyste tło
 * @retval   Brak
 */
void KS0108LCD_PutImage(uint8_t *image,
                        int16_t offsetX, int16_t offsetY, 
				        bool isTransparentBackground)
{
	uint16_t i, j;					// Pomocnicze zmienne
	int16_t x, y;					// Pomocnicze zmienne x i y
	
#ifdef KS0108LCD_FRAME_BUFF_ENABLED
	for (j = 0; j < KS0108LCD_HEIGHT / 8; j++) 
	{   
		for (i = 0; i < KS0108LCD_WIDTH; i++)
		{	
			// Obliczanie współrzędnych
			x = i - offsetX;
			y = j - offsetY;
					
			// Zapis do wyświetlacza  
			if (x >= 0 && y >= 0 &&
			    x < KS0108LCD_WIDTH && y < KS0108LCD_HEIGHT / 8)
			{
				FrameBuffer[i][j] = image[y * KS0108LCD_WIDTH + x];
			}	
			else if (!isTransparentBackground)
			{
				FrameBuffer[i][j] = 0;				
			}		
		}		
	}
#else
	for (j = 0; j < KS0108LCD_HEIGHT / 8; j++) 
	{				
		y = j - offsetY;			// Obliczanie współrzędnej y		
		KS0108LCD_GoTo(0, y);		// Ustawienie współrzędnej
									// y wyświetlacza
		for (i = 0; i < KS0108LCD_WIDTH; i++) 
		{   			
			x = i - offsetX;		// Obliczanie współrzędnej x
			
			// Zapis do wyświetlacza
			if (x >= 0 && y >= 0 &&
			    x < KS0108LCD_WIDTH && y < KS0108LCD_HEIGHT / 8)
			{					   
				KS0108LCD_WriteData(image[y * KS0108LCD_WIDTH + x]);
			}			
		}
	}	
#endif
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Wyświetlanie obrazu (z pamięci Flash)
 * @param    *image : kod obrazka
 * @param    offsetX : numer kolumny prawego górnego rogu obrazka
 * @param    offsetY : numer strony prawego górnego rogu obrazka
 * @param    isTransparentBackground : flaga oznaczająca przezroczyste tło
 * @retval   Brak
 */
void KS0108LCD_PutImage_P(uint8_t *image, 
                          int16_t offsetX, int16_t offsetY,
					      bool isTransparentBackground)
{
	uint16_t i, j;					// Pomocnicze zmienne
	int16_t x, y;					// Pomocnicze zmienne x i y
	
#ifdef KS0108LCD_FRAME_BUFF_ENABLED
	for (j = 0; j < KS0108LCD_HEIGHT / 8; j++) 
	{   
		for (i = 0; i < KS0108LCD_WIDTH; i++)
		{	
			// Obliczanie współrzędnych
			x = i - offsetX;
			y = j - offsetY;
					
			// Zapis do wyświetlacza  
			if (x >= 0 && y >= 0 &&
			    x < KS0108LCD_WIDTH && y < KS0108LCD_HEIGHT / 8)
			{
				FrameBuffer[i][j] =  
					pgm_read_byte(&image[y * KS0108LCD_WIDTH + x]);
			}	
			else if (!isTransparentBackground)
			{
				FrameBuffer[i][j] = 0;				
			}		
		}		
	}
#else
	for (j = 0; j < KS0108LCD_HEIGHT / 8; j++) 
	{				
		y = j - offsetY;			// Obliczanie współrzędnej y		
		KS0108LCD_GoTo(0, y);		// Ustawienie współrzędnej
									// y wyświetlacza
		for (i = 0; i < KS0108LCD_WIDTH; i++) 
		{   			
			x = i - offsetX;		// Obliczanie współrzędnej x
			
			// Zapis do wyświetlacza
			if (x >= 0 && y >= 0 &&
			    x < KS0108LCD_WIDTH && y < KS0108LCD_HEIGHT / 8)
			{					   
				KS0108LCD_WriteData(pgm_read_byte(&image[y * KS0108LCD_WIDTH + x]));
			}			
		}
	}	
#endif
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Zapalanie piksela zadanym kolorem
 * @param    x : współrzędna pozioma
 * @param    y : współrzędna pionowa
 * @param    color : kolor piksela
 * @retval   Brak
 */
void KS0108LCD_SetPixel(uint16_t x, uint16_t y, uint16_t color)
{
	#ifdef KS0108LCD_FRAME_BUFF_ENABLED
	
	// Sprawdzanie koloru piksela
	if (color == KS0108LCD_BLACK_COLOR)
	{
		// Zapalanie piksela
		FrameBuffer[x][y / 8] |= (1 << (y % 8));
	}
	else if (KS0108LCD_WHITE_COLOR)
	{
		// Gaszenie piksela
		FrameBuffer[x][y / 8] &= ~(1 << (y % 8));
	}
	
	#else
	
	uint8_t temp = 0;               // Zmienna pomocnicza

	KS0108LCD_GoTo(x, y / 8);       // Ustawienie współrzędnych
	KS0108LCD_ReadData();			// Czytamy cały bajt
	temp = KS0108LCD_ReadData();    // Dla pewności dwa razy
	KS0108LCD_GoTo(x, y / 8);       // Ponowne ustawienie współrzędnych

	// Sprawdzanie koloru piksela
	if (color == KS0108LCD_BLACK_COLOR)
	{
		// Zapalanie piksela
		KS0108LCD_WriteData(temp | (1 << (y % 8)));
	}
	else if (KS0108LCD_WHITE_COLOR)
	{
		// Gaszenie piksela
		KS0108LCD_WriteData(temp & ~(1 << (y % 8)));
	}
	
	#endif
}

/******************* (C) COPYRIGHT 2014 HENIUS *************** KONIEC PLIKU ***/