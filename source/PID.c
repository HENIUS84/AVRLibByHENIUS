/**
 *******************************************************************************
 * @file     PID.c
 * @author   HENIUS (Paweł Witak)
 * @version  1.01.001
 * @date     25-05-2011
 * @brief    Regulatora cyfrowy PID
 *
 * 			 Ten plik został napisany w oparciu o kod Atmela (AVR221).
 *******************************************************************************
 *
 * <h2><center>COPYRIGHT 2011 HENIUS</center></h2>
 */

/* Sekcja include ------------------------------------------------------------*/

// Pliki systemowe
#include <stdint.h>
#include <stdio.h>

// Pliki użytkownika
#include "PID.h"

/* Sekcja zmiennych ----------------------------------------------------------*/

/* Sekcja funkcji ------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/**
 * @brief    Inicjalizacja regulatora PID
 * @param    *pid : wskaźnik do struktury konfiguracyjnej regulatora
 * @retval   Brak
 */
void PID_Init(PID_t *pid)
{
	// Wartości początkowe dla regulatora
	pid->SumError = 0;
	pid->LastProcessValue = 0;

	// Limity, aby uniknąć przepełnienia
	pid->MaxError = MAX_INT / (pid->P_Factor + 1);
	pid->MaxSumError = MAX_I_TERM / (pid->I_Factor + 1);
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Funkcja obsługi regulatora
 * @param    setValue : wartość zadana
 * @param	 processValue : wartość zmierzona (procesowa)
 * @param    *pid : wskaźnik do struktury konfiguracyjnej regulatora
 * @retval   Brak
 */
int16_t PID_Handler(int16_t setValue, int16_t processValue, PID_t *pid)
{
	int16_t error;					// Aktualna wartość błędu
	int16_t p_term;					// Wartość wyjściowa członu proporcjonalnego
	int16_t d_term;					// Wartość wyjściowa członu różniczkującego
	int32_t i_term;					// Wartość wyjściowa członu całkującego
	int32_t temp; 					// Wartość tymczasowa
	int32_t result;					// Wartość wynikowa (nastawa)

	// Obliczanie aktualnej wartości błędu
	error = setValue - pid->LastProcessValue;

	// Obliczenia wartości członu proporcjonalnego i zapobieganie przepełnieniu
	if (error > pid->MaxError)
	{
		// Zabezpieczenie "od góry"
		p_term = MAX_INT;
	}
	else if (error < -pid->MaxError)
	{
		// Zabezpieczenie "od dołu"
		p_term = -MAX_INT;
	}
	else
	{
		// Wartość w zakresie
		p_term = pid->P_Factor * error;
	}

	// Obliczenia wartości członu całkującego i zapobieganie przepełnieniu
	temp = pid->SumError + error;
	if (temp > pid->MaxSumError)
	{
		// Zabezpieczenie "od góry"
		i_term = MAX_I_TERM;
		pid->SumError = pid->MaxSumError;
	}
	else if (temp < -pid->MaxSumError)
	{
		// Zabezpieczenie "od dołu"
		i_term = -MAX_I_TERM;
		pid->SumError = -pid->MaxSumError;
	}
	else
	{
		// Wartość w zakresie
		pid->SumError = temp;
		i_term = pid->I_Factor * pid->SumError;
	}

	// Obliczenia wartości członu różniczkującego
	d_term = pid->D_Factor * (pid->LastProcessValue - processValue);
	pid->LastProcessValue = processValue;

	// Przygotowanie wartości nastawy
	result = (p_term + i_term + d_term) / PID_SCALING_FACTOR;
	if (result > MAX_INT)
	{
		result = MAX_INT;
	}
	else if (result < -MAX_INT)
	{
		result = -MAX_INT;
	}

	return (int16_t)result;
}

/******************* (C) COPYRIGHT 2011 HENIUS *************** KONIEC PLIKU ***/
