/**
  ******************************************************************************
  * @file     Debug.h
  * @author   HENIUS (Paweł Witak)
  * @version  1.01.001
  * @date     18/03/2011
  * @brief    Obsługa trybu debug
  ******************************************************************************
  * 
  * <h2><center>COPYRIGHT 2011 HENIUS</center></h2>
  */

#ifndef DEBUG_H
#define DEBUG_H

/* Sekcja include ------------------------------------------------------------*/

// Pliki systemowe
#include <stdbool.h>
#include <avr/io.h>

/* Sekcja stałych, makr i definicji ------------------------------------------*/

/*! Wyświetlanie komunikatu debug (umieszcza tekst w pamięci RAM) */
#define PRINTF_DBG(msg, args...)	(DebugWrite(msg, ##args))
/*! Wyświetlanie komunikatu debug (umieszcza tekst w pamięci programu) */
#define PRINTF_DBG_P(msg, args...)	(DebugWrite_P(PSTR(msg), ##args))

/* Sekcja deklaracji ---------------------------------------------------------*/

// ---> Funkcje

// Wyświetlanie komunikatu debug (z pamięci RAM)
void DebugWrite(char *msg, ...);
// Wyświetlanie komunikatu debug (z pamięci programu)
void DebugWrite_P(char *msg, ...);
// Aktwacja trybu debug
void SetDebugMode(bool isDebugSet);

#endif								/* DEBUG_H */

/******************* (C) COPYRIGHT 2010 HENIUS *************** KONIEC PLIKU ***/
