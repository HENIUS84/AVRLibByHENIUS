/**
 *******************************************************************************
 * @file     SettingsController.h
 * @author   HENIUS (Pawe� Witak)
 * @version  1.01.001
 * @date     09-11-2012
 * @brief    Obs�uga zapisu ustawie� do pami�ci EEPROM (plik nag��wkowy)
 *******************************************************************************
 *
 * <h2><center>COPYRIGHT 2012 HENIUS</center></h2>
 */

#ifndef  SETTINGS_CONTROLLER_H_
#define  SETTINGS_CONTROLLER_H_

/* Sekcja include ------------------------------------------------------------*/

// Pliki systemowe
#include <stdbool.h>

/* Sekcja sta�ych, makr i definicji ------------------------------------------*/

// --->Sta�e
#define SETTINGS_START_ADDR	(0)		/*!< Adres pocz�tkowy do zapisu ustawie� */

/* Sekcja deklaracji ---------------------------------------------------------*/

// --->Zmienne

// --->Funkcje
// Inicjalizacja kontrolera
void SettingsCtrl_Init(uint8_t *buffer, uint16_t size);
// Zapis ustawie�;
void SettingsCtrl_SaveAll(void);
// �adowanie ustawie�
void SettingsCtrl_LoadAll(void);

#endif								/* SETTINGS_CONTROLLER_H_ */

/******************* (C) COPYRIGHT 2011 HENIUS ************** KONIEC PLIKU ****/
