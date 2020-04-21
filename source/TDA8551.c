/**
 *******************************************************************************
 * @file     TDA8551.c
 * @author   HENIUS (Paweł Witak)
 * @version  1.1.1
 * @date     30-03-2012
 * @brief    Obsługa wzmacniacza mocy TDA8551
 *******************************************************************************
 *
 * <h2><center>COPYRIGHT 2012 HENIUS</center></h2>
 */

/* Sekcja include ------------------------------------------------------------*/

// --->Pliki systemowe

#include <math.h>

// --->Pliki użytkownika

#include "TDA8551.h"

/* Sekcja zmiennych ----------------------------------------------------------*/

uint8_t VolumeLevel;				/*!< Ustawiony poziom głośności */

/* Sekcja funkcji ------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/**
 * @brief    Inicjalizacja obsługi wzmacniacza
 * @param    Brak
 * @retval   Brak
 */
void TDA8551_Init(void)
{
	// Inicjalizacja wzmacniacza
	TDA8551_MUTE();
	TDA8551_SetVolume(TDA8551_DEF_VOLUME);
	TDA8551_OPERATING();
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Generowanie pojedynczego impulsu dla wyprowadzenia VOLUME
 * @param    isLow : flaga oznaczająca niski impuls (false - wysoki impuls)
 * @retval   Brak
 */
void TDA8551_VolumeTick(bool isLow)
{
	// Ustawianie stanu niskiego lub wysokiego
	if (isLow)
	{
		VOLUME_LOW();
	}
	else
	{
		VOLUME_HIGH();	
	}
	
	// Stan wysokiej impedancji
	VOLUME_HIZ();			
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Zapamiętanie poziomu głośności
 * @param    volume : poziom głośności
 * @retval   Czy osiągniętą jedną z granic?
 */
bool TDA8551_SaveVolume(uint8_t volume)
{
	bool maxOrMin = false;
	
	// Ograniczenie od dołu i od góry
	if ((int8_t)volume >= TDA8551_STEPS_COUNT)
	{
		volume = TDA8551_STEPS_COUNT - 1;
		maxOrMin = true;
	}
	else if ((int8_t)volume < 0)
	{
		volume = 0;
		maxOrMin = true;
	}	
	
	// Dla głośności 0 nastąpi wyciszenie.
	if (!volume)
	{
		TDA8551_MUTE();
	}
	else
	{
		TDA8551_OPERATING();
	}
	
	VolumeLevel = volume;
	
	return maxOrMin;
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Ustawianie głośności
 * @param    volume : poziom głośności
 * @retval   Brak
 */
void TDA8551_SetVolume(uint8_t volume)
{
	// Indeks pomocniczy
	uint8_t idx;
	
	TDA8551_SaveVolume(volume);
	
	// Ze względu na brak informacji o aktualnej głośności 
	// należy zmniejszyć najpierw głośność do 0.
	for (idx = 0; idx < TDA8551_STEPS_COUNT; idx++)
	{
		TDA8551_VolumeTick(true);
	}
	
	// Teraz można ustawić wymaganą głośność
	for (idx = 0; idx < VolumeLevel; idx++)
	{
		TDA8551_VolumeTick(false);
	}
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Zwiększanie/zmniejszanie głośności
 * @param    isUp : flaga oznaczająca zwiększanie głośności
 * @retval   Brak
 */
void TDA8551_VolumeUpDown(bool isUp)
{
	VolumeLevel = isUp ? VolumeLevel + 1 : VolumeLevel - 1;	
	if (!TDA8551_SaveVolume(VolumeLevel))
	{		
		TDA8551_VolumeTick(!isUp);		// Zmiana głośności	
	}
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Pobieranie aktualnej głośności
 * @param    Brak
 * @retval   Poziom głośności aktualnie ustawiony
 */
uint8_t TDA8551_GetVolume(void)
{
	return VolumeLevel;
}

/******************* (C) COPYRIGHT 2012 HENIUS *************** KONIEC PLIKU ***/
