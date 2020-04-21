/**
 *******************************************************************************
 * @file     Audio.c
 * @author   HENIUS (Paweł Witak)
 * @version  1.1.1
 * @date     30-03-2012
 * @brief    Implementation of sound generator
 *******************************************************************************
 *
 * <h2><center>COPYRIGHT 2012 HENIUS</center></h2>
 */

/* Include section -----------------------------------------------------------*/

// --->System files

#include <math.h>
#include <avr/io.h>

// --->User files

#include "Audio.h"

/* Variable section ----------------------------------------------------------*/

static  CPUfrequency;						/*!< CPU frequency in MHz */

/* Function section ----------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
void Audio_Init(uint32_t cpuFreq)
{
	CPUfrequency = cpuFreq;
	
	// Konfigurowanie wyprowadzenia
	AUDIO_DDR |= AUDIO;
	
	// Konfiguracja timera
	TCCR1A |= _BV(COM1A0);
	TCCR1B |= _BV(WGM12);	 
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Starts audio generation
 * @param    None
 * @retval   None
 */
static void Audio_StartSound(void)
{
	TCCR1B |= AUDIO_PRESC_CS << CS00;
}

/*----------------------------------------------------------------------------*/
void Audio_StopSound(void)
{
	TCCR1B &= ~AUDIO_PRESC_CS << CS00;
}

/*----------------------------------------------------------------------------*/
void Audio_SetFreq(uint16_t freq)
{
	FREQ_REGISTER = CPUfrequency / (2 * (uint32_t)freq * AUDIO_PRESC) - 1;
	Audio_StartSound(); 
}	

/******************* (C) COPYRIGHT 2012 HENIUS *************** END OF FILE ****/
