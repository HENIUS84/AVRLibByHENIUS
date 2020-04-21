/**
 *******************************************************************************
 * @file     ADC.c
 * @author   HENIUS (Paweł Witak)
 * @version  1.1.1
 * @date     08-05-2011
 * @brief    Driver of ADC converter
 *******************************************************************************
 *
 * <h2><center>COPYRIGHT 2011 HENIUS</center></h2>
 */

/* Include section -----------------------------------------------------------*/

// --->System files

#include <stdint.h>
#include <stdio.h>
#include <avr/interrupt.h>

// --->User files

#include "ADC.h"

/* Variable section ----------------------------------------------------------*/

static ADC_t *AdcConfig;					/*!< Conf. structure pointer */
static volatile uint8_t ChIdx;				/*!< Number of current channel */

/* Function section ----------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
void ADC_Init(ADC_t *adcConfig)
{
	ADCresolution_t resolution;
	
	AdcConfig = adcConfig;
	ChIdx = 0;

	resolution = AdcConfig->Resolution;
	if (resolution == ADCR_11BIT ||
	    resolution == ADCR_12BIT) 
		resolution = ADCR_10BIT;

	ADCSRA = AdcConfig->SampleRate |
			_BV(ADIE);
	ADMUX = AdcConfig->Vref       |
			resolution		      |
			AdcConfig->ChannelList[ChIdx];
}

/*----------------------------------------------------------------------------*/
void ADC_StartConv(void)
{
	ADCSRA |= _BV(ADEN);
	ADCSRA |= _BV(ADSC);
}

/*----------------------------------------------------------------------------*/
void ADC_StopConv(void)
{
	ADCSRA &= ~_BV(ADEN);
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    IRQ handler of ADC
 * @param    None
 * @retval   None
 */
ISR(ADC_vect)
{
	static uint8_t oversamplingIndex = 0;
	uint8_t oversamplingFactor = 0;
		
	switch (AdcConfig->Resolution) 
	{
		case ADCR_8BIT:
			AdcConfig->Buffer[ChIdx] = ADCH;
			
			break;
		
		case ADCR_10BIT:
			AdcConfig->Buffer[ChIdx] = ADC;
			
			break;
			
		// Oversampling
		case ADCR_11BIT:
		case ADCR_12BIT:
			oversamplingFactor = 4 << (4 * (AdcConfig->Resolution - 1));
			AdcConfig->OVSbuffer[ChIdx] += ADC;
			
			if (oversamplingIndex == (oversamplingFactor - 1))
			{			
				AdcConfig->Buffer[ChIdx] = 
					AdcConfig->OVSbuffer[ChIdx] >> AdcConfig->Resolution;					
				AdcConfig->OVSbuffer[ChIdx] = 0;
			}
					
			break;
	}

	// Next channel
	ChIdx++;
	ChIdx %= AdcConfig->ChannelAmount;
	ADMUX &= ADC_CHANNEL_MASK;
	ADMUX |= AdcConfig->ChannelList[ChIdx];
		
	// Callbacks
	if (!ChIdx && AdcConfig->OnCompleted)
	{
		oversamplingIndex++;
		oversamplingIndex %= oversamplingFactor;		
		
		if (oversamplingFactor && !oversamplingIndex) 
			AdcConfig->OnCompleted();
		else
			AdcConfig->OnCompleted();
	}
	
	if ((AdcConfig->Mode == ADCM_SINGLE && ChIdx) ||
	     AdcConfig->Mode == ADCM_AUTO ||
	    (oversamplingFactor && oversamplingIndex))
		ADC_StartConv();		
}

/*----------------------------------------------------------------------------*/
void ADC_Deinit(void)
{
	ChIdx = 0;
	ADCSRA &= ~(_BV(ADEN) | _BV(ADIE));
}

/******************* (C) COPYRIGHT 2011 HENIUS *************** END OF FILE ****/
