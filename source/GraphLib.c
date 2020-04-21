/**
 *******************************************************************************
 * @file     GraphLib.c
 * @author   HENIUS (Paweł Witak)
 * @version  1.1.1
 * @date     23/08/2011
 * @brief    Implementacja biblioteki graficznej
 *******************************************************************************
 *
 * <h2><center>COPYRIGHT 2011 HENIUS</center></h2>
 */

/* Sekcja include ------------------------------------------------------------*/

// --->Pliki systemowe

#include <stdio.h>
#include <string.h>
#include <avr/pgmspace.h>

// --->Pliki użytkownika

#include "GraphLib.h"

/* Sekcja zmiennych ----------------------------------------------------------*/

/*! Tablica szerokości znaków */
uint8_t CharsWidthTable[MAX_WORD_LENGTH];	
/*! Wskaźnik do funkcji ustawiającej piksel */
void (*SetPixel)(uint16_t x, uint16_t y, Color_t pixelColor);

/* Sekcja funkcji ------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/**
 * @brief    Inicjalizacja biblioteki graficznej
 * @param    setPixel : wskaźnik do funkcji ustawiającej piksel
 * @retval : Postać znaku Latin-2
 */
void InitGraphLib(void(*setPixel)(uint16_t x, uint16_t y, Color_t pixelColor))
{
	SetPixel = setPixel;	
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Konwersja znaku do formatu Latin-2
 * @param    c : znak
 * @retval   Postać znaku Latin-2
 */
uint8_t ConvertToLatin2(uint16_t c)
{
	uint16_t result = c;

	// Czy to polski znak?
	switch (c)
	{
		case 'Ą':
			result = P_LETTER_A;

			break;

		case 'Ć':
			result = P_LETTER_C;

			break;

		case 'Ę':
			result = P_LETTER_E;

			break;

		case 'Ł':
			result = P_LETTER_L;

			break;

		case 'Ń':
			result = P_LETTER_N;

			break;

		case 'Ó':
			result = P_LETTER_O;

			break;

		case 'Ś':
			result = P_LETTER_S;

			break;

		case 'Ź':
			result = P_LETTER_X;

			break;

		case 'Ż':
			result = P_LETTER_Z;

			break;

		case 'ą':
			result = P_LETTER_a;

			break;

		case 'ć':
			result = P_LETTER_c;

			break;

		case 'ę':
			result = P_LETTER_e;

			break;

		case 'ł':
			result = P_LETTER_l;

			break;

		case 'ń':
			result = P_LETTER_n;

			break;

		case 'ó':
			result = P_LETTER_o;

			break;

		case 'ś':
			result = P_LETTER_s;

			break;

		case 'ź':
			result = P_LETTER_x;

			break;

		case 'ż':
			result = P_LETTER_z;

			break;
	}

	return result;
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Konwersja znaku z formatu Latin-2
 * @param    c : znak w formacie Latin-2
 * @retval   Znak
 */
uint16_t ConvertFromLatin2(uint8_t c)
{
	uint16_t result = c;

	switch (c)
	{
		case P_LETTER_a:
			result = 'ą';

			break;

		case P_LETTER_A:
			result = 'Ą';

			break;

		case P_LETTER_c:
			result = 'ć';

			break;

		case P_LETTER_C:
			result = 'Ć';

			break;

		case P_LETTER_e:
			result = 'ę';

			break;

		case P_LETTER_E:
			result = 'Ę';

			break;

		case P_LETTER_l:
			result = 'ł';

			break;

		case P_LETTER_L:
			result = 'Ł';

			break;

		case P_LETTER_n:
			result = 'ń';

			break;

		case P_LETTER_N:
			result = 'Ń';

			break;

		case P_LETTER_o:
			result = 'ó';

			break;

		case P_LETTER_O:
			result = 'Ó';

			break;

		case P_LETTER_s:
			result = 'ś';

			break;

		case P_LETTER_S:
			result = 'Ś';

			break;

		case P_LETTER_x:
			result = 'ź';

			break;

		case P_LETTER_X:
			result = 'Ź';

			break;

		case P_LETTER_z:
			result = 'ż';

			break;

		case P_LETTER_Z:
			result = 'Ż';

			break; 
	}

	return result;
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Pobieranie tablicy szerokości znaków w słowie
 * @param    *text : tekst
 * @param    *font : typ czcionki
 * @param    charSpace : odstęp między znakami
 * @retval   Długość łańcucha
 */
void GetCharsWidthTable(uint8_t *text,
                        Font_t *font,
                        uint8_t charSpace)
{
	// Liczba znaków w łańcuchu
    uint8_t charactersCount = strlen((char*)text);
    // Indeks
    uint8_t idx;
    // Indeks znaku
    uint16_t charIdx;

    for (idx = 0; idx < MAX_WORD_LENGTH; idx++)
    {
		CharsWidthTable[idx] = 0;
		
		if (idx < charactersCount)
		{
			// Pobieranie indeksu i rozmiaru znaku
#ifdef GL_AVR
			charIdx = pgm_read_byte(&font->Index[ConvertToLatin2(text[idx])]);
			CharsWidthTable[idx] += pgm_read_byte(&font->Width[charIdx]) + 
			                        charSpace;
#else
			charIdx = font->Index[ConvertToLatin2(text[idx])];
			CharsWidthTable[idx] += font->Width[charIdx] + charSpace;
#endif
		}	
    }
    CharsWidthTable[idx - 1] -= charSpace;
}	

/*----------------------------------------------------------------------------*/
/**
 * @brief    Pomiar długości aktualnie przetwarzanego łańcucha
 * @param    Brak
 * @retval   Długość łańcucha
 */
uint16_t MeasureString(void)
{	
	uint16_t stringLength = 0;		// Długość łańcucha w pikselach 
	uint8_t idx = 0;				// Indeks

	while (idx < MAX_WORD_LENGTH &&
	       CharsWidthTable[idx])
	{
		stringLength += CharsWidthTable[idx++];
	} 

	return stringLength;
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Pobieranie słowa danego znaku
 * @param    x : współrzędna X
 * @param    wordIdx : numer słowa (w pionie)
 * @param    character : znak
 * @param    *font : typ czcionki znaku
 * @retval   Słowo znaku
 */
uint32_t GetCharWord(uint16_t x, uint16_t wordIdx,
                     uint8_t character,
                     Font_t *font)
{
	// Pobrane słowo
    uint32_t word = 0;
    // Indeks znaku w tabelach czcionki
	uint8_t charIndex = 
	#ifdef GL_AVR
		pgm_read_byte(&font->Index[ConvertToLatin2(character)]);
	#else
		font->Index[ConvertToLatin2(character)];
	#endif
    // Indeks poszukiwanego słowa
    uint16_t searchWordIndex;
    // Liczba bajtów pojedynczego słowa czcionki
    uint8_t bytesInWord = font->WordSize > 8 ? font->WordSize / 8 : 1;
    uint8_t idx = 0;

    // Pobieranie indeksu poszukiwanego słowa
	searchWordIndex = 	
	#ifdef GL_AVR	
		(wordIdx * pgm_read_byte(&font->Width[charIndex]) + x) * 
		bytesInWord;		
	#else	
		(wordIdx * font->Width[charIndex] + x) * bytesInWord;		
	#endif
	
    // Kompletowanie słowa
    for (idx = 0; idx < bytesInWord; idx++)
    {
        word <<= 8;
		word |= 
		#ifdef GL_AVR
			pgm_read_byte(
				&font->Data[searchWordIndex + 
				pgm_read_word(&font->Offset[charIndex]) + idx]);
		#else
			font->MBData[searchWordIndex + font->Offset[charIndex] + idx];
		#endif
		
    }

    return word;
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Konwersja koloru 16 bitowego na składowe RGB
 * @param    color : kolor 16-bitowy
 * @retval   Składowe RGB koloru
 */
RGB_t ConvertColorToRGB(Color_t color)
{
	RGB_t result;					// Wynik

	result.Red = ((color & 0xF800) >> 11) * 0xFF / 0x1F;
	result.Green = ((color & 0x07E0) >> 5) * 0xFF / 0x3F;
	result.Blue = (color & 0x001F) * 0xFF / 0x1F;

	return result;
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Konwersja składowych RGB na kolor
 * @param    rgb : składowe RGB koloru
 * @retval : Kolor 16-bitowy
 */
Color_t ConvertRGBToColor(RGB_t rgb)
{
	Color_t result;

	result = ((rgb.Red >> 3) << 11) + ((rgb.Green >> 2) << 5) + (rgb.Blue >> 3);

	return result;
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Interpolacja dwukolorowa
 * @param    colorA : pierwszy kolor
 * @param    colorB : drugi kolor
 * @param    pos : pozycja
 * @param    max : wartość maksymalna
 * @retval   Kolor gradientu
 */
Color_t Interpolate2Colors(Color_t colorA, Color_t colorB, 
                           uint16_t pos, uint16_t max)
{
	RGB_t resRGB; 					// Kolor gradientu (w postaci RGB)
	RGB_t aRGB, bRGB;           	// Składowe RGB kolorów gradientu

	// Obliczanie składowych RGB kolorów gradientu
	aRGB = ConvertColorToRGB(colorA);
	bRGB = ConvertColorToRGB(colorB);

	resRGB.Red = (bRGB.Red - aRGB.Red) * pos / max + aRGB.Red;
	resRGB.Green = (bRGB.Green - aRGB.Green) * pos / max + aRGB.Green;
	resRGB.Blue = (bRGB.Blue - aRGB.Blue) * pos / max + aRGB.Blue;

	return ConvertRGBToColor(resRGB);
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Interpolacja trójkolorowa
 * @param    colorA : pierwszy kolor
 * @param    colorB : drugi kolor
 * @param    colorC : trzeci kolor
 * @param    pos : pozycja
 * @param    max : wartość maksymalna
 * @retval   Kolor gradientu
 */
Color_t Interpolate3Colors(Color_t colorA, Color_t colorB, Color_t colorC, 
                           uint16_t pos, uint16_t max)
{
	Color_t result;

	if (pos < max / 2)
	{
		result = Interpolate2Colors(colorA, colorB, pos, max / 2);
	}
	else
	{
		result = Interpolate2Colors(colorB, colorC, pos - max / 2, max / 2);
	}

	return result;
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Rysowanie komponentu (z pamięci RAM) TextBlock
 * @param    *textBlock : obiekt komponentu
 * @param    *text : tekst
 * @retval   Brak
 */
void PutTextBlock(TextBlock_t *textBlock, uint8_t *text)
{
	uint16_t x, y;					// Zmienne pomocnicze
	Point_t lcdPoint;				// Aktualne wartości współrzędnych ekranu
	Color_t pixelColor = 0;			// Kolor piksela
	Point_t textLocation = { 0, 0 };// Lokalizacja tekstu
	uint16_t textLength;			// Długość tekstu            
	Size_t totalTextBlockSize =		// Maksymalny rozmiar TextBlock'a (z ramka)            
	{
		.Width = textBlock->Size.Width + textBlock->BorderThickness.Left +
		         textBlock->BorderThickness.Right,
		.Height = textBlock->Size.Height + textBlock->BorderThickness.Top +
		          textBlock->BorderThickness.Bottom
	};
	bool isPaintedBackground = false;	// Flaga oznaczająca malowanie tła
	uint16_t sumOfWidths;				// Suma szerokości znaków
	uint8_t charIdx = 0;				// Indeks znaku
	uint8_t wordIdx;					// Indeks słowa
	uint32_t currentWord = 0;			// Aktualne słowo czcionki	
	// Liczba znaków w danym tekście
	uint8_t charsNumber = strlen((char*)text);
	Font_t font;						// Dana czcionka

	// Pobieranie czcionki
#ifdef GL_AVR	
	memcpy_P((void *)&font, textBlock->Font, sizeof(Font_t));
#else
	font = *textBlock->Font;
#endif	

	// Obliczanie długości tekstu
	GetCharsWidthTable(text,
	                   &font,
					   textBlock->CharSpacing);
	textLength = MeasureString();
	sumOfWidths = CharsWidthTable[0];
	
	// Współrzędna pozioma
	switch (textBlock->HorizontalAlignment)
	{
		case HA_LEFT:
			textLocation.X = textBlock->Location.X;

			break;
		case HA_CENTER:
			textLocation.X = (int16_t)(textBlock->Size.Width - textLength) / 2 + 
			                  textBlock->Location.X;

			break;
		case HA_RIGHT:
			textLocation.X = textBlock->Location.X + textBlock->CharSpacing +
			                 textBlock->Size.Width - textLength;

			break;
	}

	// Współrzędna pionowa
	switch (textBlock->VerticalAlignment)
	{
		case VA_TOP:
			textLocation.Y = textBlock->Location.Y;

			break;

		case VA_CENTER:
			textLocation.Y = (int16_t)(textBlock->Size.Height - 
			                           font.Height) / 2 + 
							           textBlock->Location.Y;

			break;

		case VA_BOTTOM:
			textLocation.Y = textBlock->Location.Y + 
			                 textBlock->Size.Height - font.Height;

			break;
	}

	// Uwzględnianie offsetu tekstu
    textLocation.X += textBlock->TextOffset.X;
    textLocation.Y += textBlock->TextOffset.Y;

	for (x = 0; x < totalTextBlockSize.Width; x++)
	{
		// Obliczanie współrzędnej x ekranu
		lcdPoint.X = x + textBlock->Location.X - 
		             textBlock->BorderThickness.Left;
		
		// Obliczanie indeksu aktualnie przetwarzanego zanku
        if (lcdPoint.X >= sumOfWidths + textLocation.X)
        {
            charIdx++;

            if (charIdx < charsNumber)
            {
                sumOfWidths += CharsWidthTable[charIdx];
            }
        }
		
		wordIdx = 0;				// Numeracja słów od początku
		
		for (y = 0; y < totalTextBlockSize.Height; y++)
		{
			// Obliczanie wpółrzędnej y ekranu
			lcdPoint.Y = y + textBlock->Location.Y - 
			             textBlock->BorderThickness.Top;

			if (y < textBlock->BorderThickness.Top ||
				y > (totalTextBlockSize.Height - 
				     textBlock->BorderThickness.Bottom - 1) ||
				x < textBlock->BorderThickness.Left ||
				x > (totalTextBlockSize.Width - 
				     textBlock->BorderThickness.Right - 1))
			{
				// Rysowanie ramki
				isPaintedBackground = false;
				pixelColor = textBlock->BorderColor;
			}
			else
			{
				// Rysowanie tła lub znaków
				if (lcdPoint.X >= textLocation.X &&
					lcdPoint.X < (textLocation.X + textLength) &&
					lcdPoint.Y >= textLocation.Y &&
					lcdPoint.Y < (textLocation.Y + font.Height))
				{
					// Czy pobrać kolejne słowo?
                    if (lcdPoint.Y >= 
					   (wordIdx * font.WordSize + textLocation.Y))
                    {
                        currentWord = 
							GetCharWord(lcdPoint.X - textLocation.X - 
							            sumOfWidths + CharsWidthTable[charIdx], 
										wordIdx,
                                        text[charIdx],
                                        &font);
                        wordIdx++;                                
                    }
					
					// Obszar tekstu
					if ((currentWord & 
					    (1 << ((lcdPoint.Y - textLocation.Y) % 
						font.WordSize))) &&
                        lcdPoint.X < sumOfWidths + textLocation.X - 
						textBlock->CharSpacing)
					{
						// Piksel czcionki
						isPaintedBackground = false;
						pixelColor = textBlock->TextColor; 
					}
					else
					{
						// Piksel tła
						isPaintedBackground = true;
						pixelColor = textBlock->BackgroundColor;
					}
				}
				else
				{
					// Rysowanie tła
					isPaintedBackground = true;
					pixelColor = textBlock->BackgroundColor;
				}
			}

			if ((!isPaintedBackground || (isPaintedBackground && 
			                              !textBlock->IsTransparentBackground)) 
                && (lcdPoint.X >= 0 && lcdPoint.Y >= 0))
			{				
				// Przezroczyste tło nie będzie rysowane.
				SetPixel(lcdPoint.X, lcdPoint.Y, pixelColor);
			}
		}
	}
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Rysowanie komponentu (z pamięci Flash) TextBlock
 * @param    *textBlock : obiekt komponentu
 * @param    *text : tekst
 * @retval   Brak
 */
#ifdef GL_AVR
void PutTextBlock_P(const TextBlock_t *textBlock, uint8_t *text)
{
	TextBlock_t component;			// Komponent		
	
	// Pobieranie opisu komponentu
	memcpy_P(&component, textBlock, sizeof(TextBlock_t));		
	
	// Rysowanie komponentu
	PutTextBlock(&component, text);
}
#endif

/*----------------------------------------------------------------------------*/
/**
 * @brief    Rysowanie okręgu (z pamięci RAM)
 * @param    *circle : obiekt komponentu
 * @retval   Brak
 */
void PutCircle(Circle_t *circle)
{
	// Współrzędne
	int16_t x = 0, y = circle->Radius; 
	// Zmienne decyzyjne
	int16_t d_old = 5 - 4 * circle->Radius, d_new;      

	d_new = d_old;

	// Zapalmy punkty o współrzędnych kolejno 
	// (0 + circle.Location.X, R + circle.Location.Y), 
	// (0 + circle.Location.X, - R + circle.Location.Y), 
	// (R + circle.Location.X, 0 + circle.Location.Y), 
	// (- R + circle.Location.X,0 + circle.Location.Y).
	SetPixel(circle->Location.X, circle->Location.Y - circle->Radius,
			 circle->Color);
	SetPixel(circle->Location.X, circle->Location.Y + circle->Radius,
			 circle->Color);
	SetPixel(circle->Radius + circle->Location.X, circle->Location.Y,
			 circle->Color);
	SetPixel(circle->Location.X - circle->Radius, circle->Location.Y,
			 circle->Color);

	// Ustalmy pozostałe punkty.
	while (y > x)                   // Róbmy obliczenia dopóki y > x.
	{
		if (d_new >= 0)             // Wybierzmy punkt południowy.
		{
			d_new += 8 * (x - y) + 20;
			x++; y--;
		}
		else
		{
			d_new += 8 * x + 12;
			x++;
		}

		// Wykreślamy tylko punkty dla 1/8 okręgu, pozostałe są  ustalane
		// poprzez symetrię.
		// Sprawdzam kolor ołówka.
		SetPixel(x + circle->Location.X, circle->Location.Y - y,
				 circle->Color);
		SetPixel(x + circle->Location.X, circle->Location.Y + y,
				 circle->Color);
		SetPixel(y + circle->Location.X, circle->Location.Y + x,
				 circle->Color);
		SetPixel(y + circle->Location.X, circle->Location.Y - x,
				 circle->Color);
		SetPixel(-x + circle->Location.X, circle->Location.Y + y,
				 circle->Color);
		SetPixel(-y + circle->Location.X, circle->Location.Y + x,
				 circle->Color);
		SetPixel(-y + circle->Location.X, circle->Location.Y - x,
				 circle->Color);
		SetPixel(-x + circle->Location.X, circle->Location.Y - y,
				 circle->Color);
	}
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Rysowanie okręgu (z pamięci Flash)
 * @param    *circle : obiekt komponentu
 * @retval   Brak
 */
void PutCircle_P(Circle_t *circle)
{
	Circle_t component;				// Komponent
	
	// Pobieranie opisu komponentu
	memcpy_P(&component, circle, sizeof(Circle_t));		
	
	// Rysowanie komponentu
	PutCircle(&component);
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Rysowanie lini (z pamięci RAM)
 * @param    *line : obiekt komponentu
 * @retval   Brak
 */
void PutLine(Line_t *line)
{

	int16_t yinc;                   // Wielkość skoku y
	Point_t currPoint = line->From; // Aktualny punkt
	int16_t a = line->To.Y - line->From.Y;
	int16_t b = line->To.X - line->From.X;
	int16_t d_old = 2 * a - b;      // Zmienne decyzyjna początkowa
	int16_t d_new = d_old;          // Zmienne decyzyjna końcowa

	// Warunek: line.To.X > line.From.X
	if (line->From.X > line->To.X)
	{
		line->From = line->To;
		line->To = currPoint;
	}

	if (a < 0)                      // Funkcja rosnąca 
	{
		yinc = -1;
		a = -a;
	}
	else                            // Funkcja malejąca
	{
		yinc = 1;
	}

	if (line->From.X < line->To.X)
	{
		// Rysowanie punktu początkowego
		SetPixel(currPoint.X, currPoint.Y,
				 line->Color);

		do
		{
			if (d_new > 0)                      // Wybór punktu zgodnie ze zmienną
			{                                   // decyzyjną
				d_new = d_new + 2 * a - 2 * b;	// Inkrementacja zmiennej decyzyjnej
				currPoint.Y += yinc;
			}
			else                                // Wline.To.Yór punktu zgodnie ze zmienną
			{                                   // decyzyjną
				d_new = d_new + 2 * a;          // Inkrementacja zmiennej decyzyjnej
			}

			currPoint.X++;

			SetPixel(currPoint.X, currPoint.Y,
					 line->Color);
		}
		while (currPoint.X != line->To.X);
	}
	else if (line->From.X == line->To.X)        // Została wybrana linia pionowa
	{
		do
		{
			SetPixel(currPoint.X, currPoint.Y++,
					 line->Color);
		}
		while (currPoint.Y <= line->To.Y);
	}
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Rysowanie lini (z pamięci Flash)
 * @param    *line : obiekt komponentu
 * @retval   Brak
 */
void PutLine_P(Line_t *line)
{
	Line_t component;				// Komponent
	
	// Pobieranie opisu komponentu
	memcpy_P(&component, line, sizeof(Line_t));		
	
	// Rysowanie komponentu
	PutLine(&component);
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Rysowanie prostokąta (z pamięci RAM)
 * @param    *rectangle : obiekt komponentu
 * @retval   Brak
 */
void PutRectangle(Rectangle_t *rectangle)
{
	uint16_t x, y;					// Zmienne pomocnicze

	for (x = 0; x < rectangle->Size.Width; x++)
	{
		for (y = 0; y < rectangle->Size.Height; y++)
		{
			if (y == 0 ||
				y == (rectangle->Size.Height - 1) ||
				x == 0 ||
				x == (rectangle->Size.Width - 1))
			{
				// Rysowanie ramki
				SetPixel(x + rectangle->Location.X, y + rectangle->Location.Y,
						 rectangle->LineColor);
			}
			else if (!rectangle->IsTransparentBackground)
			{
				// Rysowanie tła
				SetPixel(x + rectangle->Location.X, y + rectangle->Location.Y,
						 rectangle->BackgroundColor);
			}
		}
	}
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Rysowanie prostokąta (z pamięci Flash)
 * @param    *rectangle : obiekt komponentu
 * @retval   Brak
 */
void PutRectangle_P(Rectangle_t *rectangle)
{
	Rectangle_t component;			// Komponent
	
	// Pobieranie opisu komponentu
	memcpy_P(&component, rectangle, sizeof(Rectangle_t));		
	
	// Rysowanie komponentu
	PutRectangle(&component);
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Rysowanie paska postępu (z pamięci RAM)
 * @param    *progressBar : obiekt komponentu
 * @param    value : aktualna wartość paska
 * @retval   Brak
 */
void PutProgressBar(ProgressBar_t *progressBar, uint16_t val)
{
	uint16_t x, y;					// Zmienne pomocnicze
	int16_t currentX, currentY;		// Aktualne wartości współrzędnych ekranu
	uint16_t pixelColor;			// Kolor aktualnego piksela
	uint16_t horizontalGradientColor;	// Kolor gradientu poziomego
	Size_t totalProgressBarSize =	// Maksymalny rozmiar paska (z ramka)            
	{
		.Width = progressBar->Size.Width + 2 * progressBar->BorderThickness,
		.Height = progressBar->Size.Height + 2 * progressBar->BorderThickness
	};
	bool isPaintedBackground = false;	// Flaga oznaczająca malowanie tła
	bool isMonochrome = false;

	// Sprawdzanie czy kolor paska jest monochromatyczny
	if (progressBar->CentralColor == progressBar->EndColor &&
		progressBar->CentralColor == progressBar->StartColor &&
		progressBar->EndColor == progressBar->StartColor)
	{
		isMonochrome = true;
	}

	for (x = 0; x < totalProgressBarSize.Width; x++)
	{
		for (y = 0; y < totalProgressBarSize.Height; y++)
		{
			// Obliczanie wpółrzędnych ekranu
			currentX = x + progressBar->Location.X - 
			               progressBar->BorderThickness;
			currentY = y + progressBar->Location.Y - 
			               progressBar->BorderThickness;

			if (y < progressBar->BorderThickness ||
				y > (totalProgressBarSize.Height - 
				     progressBar->BorderThickness - 1) ||
				x < progressBar->BorderThickness ||
				x > (totalProgressBarSize.Width - 
				     progressBar->BorderThickness - 1))
			{
				// Rysowanie ramki
				isPaintedBackground = false;
				pixelColor = progressBar->BorderColor;
			}
			else
			{
				// Rysowanie tła lub paska
				if (x <= progressBar->Size.Width * val /
				         progressBar->Maximum)
				{
					// Obszar paska                            
					isPaintedBackground = false;

					if (isMonochrome)
					{
						// Kiedy wszystkie kolory są identyczne 
						// interpolacja nie jest potrzebna.
						pixelColor = progressBar->StartColor;
					}
					else
					{
						horizontalGradientColor = 
							Interpolate2Colors(progressBar->StartColor,
											   progressBar->EndColor,
											   x - progressBar->BorderThickness,
											   progressBar->Size.Width);
						pixelColor = 
							Interpolate3Colors(horizontalGradientColor,
											   progressBar->CentralColor,
											   horizontalGradientColor,
											   y - progressBar->BorderThickness,
											   progressBar->Size.Height);
					}
				}
				else
				{
					// Rysowanie tła
					isPaintedBackground = true;
					pixelColor = progressBar->BackgroundColor;
				}
			}

			if ((!isPaintedBackground || 
			    (isPaintedBackground && !progressBar->IsTransparentBackground)) 
				&& (currentX >= 0 && currentY >= 0))
			{
				// Przezroczyste tło nie będzie rysowane.
				SetPixel(currentX, currentY, pixelColor);
			}
		}
	}
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Rysowanie paska postępu (z pamięci Flash)
 * @param    *progressBar : obiekt komponentu
 * @param    value : aktualna wartość paska
 * @retval   Brak
 */
#ifdef GL_AVR
void PutProgressBar_P(ProgressBar_t *progressBar, uint16_t val)
{
	ProgressBar_t component;		// Komponent
	
	// Pobieranie opisu komponentu
	memcpy_P(&component, progressBar, sizeof(ProgressBar_t));	
	
	// Rysowanie komponentu
	PutProgressBar(&component, val);
}	
#endif

/*----------------------------------------------------------------------------*/
/**
 * @brief    Umieszczanie na ekranie obrazka (z pamięci RAM)
 * @param    *image : tablica z kodem obrazka
 * @param    x : współrzędna x lewego górnego rogu obrazka
 * @param    y : współrzędna y lewego górnego rogu obrazka
 * @retval   Brak
 */
void PutImage(uint8_t *image, int16_t x, int16_t y)
{
	BitmapHeader_t *header;			// Nagłówek obrazka
	uint16_t i, j;					// Zmienne pomocnicze
	uint8_t currentByte = 0;		// Aktualnie przetwarzany bajt
	Color_t color = 0;				// Aktualny kolor piksela
	
	// Pobieranie nagłówka obrazka	
	header = (BitmapHeader_t *)image;
	
	// Pętla wyświetlająca	
	for (j = 0; j < header->Size.Width; j++)
	{
		for (i = 0; i < header->Size.Width; i++)
		{
			// Jaka to paleta kolorów?
			switch (header->ColorPalette)
			{
				// --->Paleta 1 bitowa
				case 1:	
					// Czy pobrać kolejny bajt?
					if (!(i % 8))
					{
						currentByte = image[(i / 8) * header->Size.Width + j + 
										    sizeof(BitmapHeader_t)];
					}
					
					color = currentByte & _BV(i % 8) ? SC_BLACK : SC_WHITE;
			
					break;	
				
				// --->Paleta 16 bitowa	
				case 16:
					color = 
						*((uint16_t *)&image[((i / 8) * header->Size.Width + j) * 
						                     2 + sizeof(BitmapHeader_t)]);
					
					break;
			}	
			
			// Ustawianie piksela
			SetPixel(j + x, i + y, color);		
		}		
	}
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Umieszczanie na ekranie obrazka (z pamięci Flash)
 * @param    *image : tablica z kodem obrazka
 * @param    x : współrzędna x lewego górnego rogu obrazka
 * @param    y : współrzędna y lewego górnego rogu obrazka
 * @retval   Brak
 */
#ifdef GL_AVR
void PutImage_P(uint8_t *image, int16_t x, int16_t y)
{
	BitmapHeader_t header;			// Nagłówek obrazka
	uint16_t i, j;					// Zmienne pomocnicze
	uint8_t currentByte = 0;		// Aktualnie przetwarzany bajt
	Color_t color = 0;				// Aktualny kolor piksela
	
	// Pobieranie nagłówka obrazka	
	memcpy_P(&header, image, sizeof(BitmapHeader_t));
	
	// Pętla wyświetlająca	
	for (j = 0; j < header.Size.Width; j++)
	{
		for (i = 0; i < header.Size.Width; i++)
		{
			// Jaka to paleta kolorów?
			switch (header.ColorPalette)
			{
				// --->Paleta 1 bitowa
				case 1:	
					// Czy pobrać kolejny bajt?
					if (!(i % 8))
					{
						currentByte = 
							pgm_read_byte(&image[(i / 8) * header.Size.Width + j + 
												 sizeof(BitmapHeader_t)]);
					}
					
					color = currentByte & _BV(i % 8) ? SC_BLACK : SC_WHITE;
			
					break;	
				
				// --->Paleta 16 bitowa	
				case 16:
					color = 
						pgm_read_word(&image[((i / 8) * header.Size.Width + j) * 
						                     2 + sizeof(BitmapHeader_t)]);
					
					break;
			}	
			
			// Ustawianie piksela
			SetPixel(j + x, i + y, color);		
		}		
	}
}
#endif

/******************* (C) COPYRIGHT 2011 HENIUS *************** KONIEC PLIKU ***/
