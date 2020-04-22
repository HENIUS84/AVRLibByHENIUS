/**
 *******************************************************************************
 * @file     PID.h
 * @author   HENIUS (Paweł Witak)
 * @version  1.01.001
 * @date     25-05-2011
 * @brief    Regulatora cyfrowy PID (plik nagłówkowy)
 *
 * 			 Ten plik został napisany w oparciu o kod Atmela (AVR221).
 *******************************************************************************
 *
 * <h2><center>COPYRIGHT 2011 HENIUS</center></h2>
 */

#ifndef  PID_H_
#define  PID_H_

/* Sekcja include ------------------------------------------------------------*/

// Pliki systemowe

/* Sekcja stałych, makr i definicji ------------------------------------------*/

// Stałe
/*! Stała skalująca wartość nastawy regulatora */
#define PID_SCALING_FACTOR	(128)
// Maksymalne wartości zmiennych
/*! Maksymalna wartość INT */
#define MAX_INT         	(INT16_MAX)
/*! Maksymalna wartość LONG */
#define MAX_LONG        	(INT32_MAX)
/*! Maksymalna wartość współczynnika członu całkującego */
#define MAX_I_TERM      	(MAX_LONG / 2)

/* Sekcja deklaracji ---------------------------------------------------------*/

// --->Zmienne

// --->Typy
/**
 * @brief Konfiguracja i charakterystyka regulatora PID
 */
typedef struct
{
	int16_t LastProcessValue;	    /*!< Ostatnia wartość procesowa */
	int32_t SumError;				/*!< Suma błędów */
	int16_t P_Factor;				/*!< Współczynnik członu proporcjonalnego */
	int16_t I_Factor;				/*!< Współczynnik członu całkującego */
	int16_t D_Factor;				/*!< Współczynnik członu różniczkującego */
	int16_t MaxError;				/*!< Maksymalny dopuszczalny błąd */
	int32_t MaxSumError;		    /*!< Maksymalna wartość SumError */
}PID_t;

// --->Funkcje
// Inicjalizacja regulatora PID
void PID_Init(PID_t *pid);
// Funkcja obsługi regulatora
int16_t PID_Handler(int16_t setValue, int16_t processValue, PID_t *pid);

#endif								/* PID_H_ */

/******************* (C) COPYRIGHT 2011 HENIUS ************** KONIEC PLIKU ****/
