/**
 *******************************************************************************
 * @file     SettingsController.h
 * @author   HENIUS (Pawe³ Witak)
 * @version  1.01.001
 * @date     09-11-2012
 * @brief    Obs³uga zapisu ustawieñ do pamiêci EEPROM (plik nag³ówkowy)
 *******************************************************************************
 *
 * <h2><center>COPYRIGHT 2012 HENIUS</center></h2>
 */

#ifndef  SETTINGS_CONTROLLER_H_
#define  SETTINGS_CONTROLLER_H_

/* Sekcja include ------------------------------------------------------------*/

// Pliki systemowe
#include <stdbool.h>

/* Sekcja sta³ych, makr i definicji ------------------------------------------*/

// --->Sta³e
#define SETTINGS_START_ADDR	(0)		/*!< Adres pocz¹tkowy do zapisu ustawieñ */

/* Sekcja deklaracji ---------------------------------------------------------*/

// --->Zmienne

// --->Funkcje
// Inicjalizacja kontrolera
void SettingsCtrl_Init(uint8_t *buffer, uint16_t size);
// Zapis ustawieñ;
void SettingsCtrl_SaveAll(void);
// £adowanie ustawieñ
void SettingsCtrl_LoadAll(void);

#endif								/* SETTINGS_CONTROLLER_H_ */

/******************* (C) COPYRIGHT 2011 HENIUS ************** KONIEC PLIKU ****/
