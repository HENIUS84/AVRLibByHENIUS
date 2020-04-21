/**
 *******************************************************************************
 * @file     ADC.h
 * @author   HENIUS (Paweł Witak)
 * @version  1.01.001
 * @date     08-05-2011
 * @brief    Driver of ADC converter (header file)
 *******************************************************************************
 *
 * <h2><center>COPYRIGHT 2011 HENIUS</center></h2>
 */

#ifndef  ADC_H_
#define  ADC_H_

/* Include section -----------------------------------------------------------*/

// --->System files

#include <avr/io.h>

/* Macros, constants and definitions section ---------------------------------*/

//--->Constants

#define ADC_CHANNEL_MASK	(~0x1F)			/*!< Bit mask for ADC channel */

// --->Macros

/* Declaration section -------------------------------------------------------*/

// --->Types

/**
 * @brief Measurement mode
 */
typedef enum
{
	ADCM_SINGLE,							/*!< Single measurement */
	ADCM_AUTO        						/*!< Continuous measurement */
}ADCmode_t;

/**
 * @brief Sample resolution
 */
typedef enum
{
	ADCR_10BIT = 0,					/*!< 8-bit resolution */
	ADCR_8BIT  = _BV(ADLAR),		/*!< 10-bit resolution */
	ADCR_11BIT = 1,					/*!< 11-bit resolution (oversampling) */
	ADCR_12BIT = 2,					/*!< 12-bit resolution (oversampling) */
}ADCresolution_t;

/**
 * @brief Sampling frequency
 */
typedef enum
{
#if F_CPU >= 8000000
	ADCSR_77KSPS   = 3,				/*!< 77k sample/s */
	ADCSR_39KSPS   = 4,             /*!< 39k sample/s */
	ADCSR_19200SPS = 5,             /*!< 19200 sample/s */
	ADCSR_9600SPS  = 6,             /*!< 9600 sample/s */
	ADCSR_4800SPS  = 7              /*!< 4800 sample/s */
#else
	ADCSR_77KSPS   = 2,
	ADCSR_39KSPS   = 2,
	ADCSR_19200SPS = 4,
	ADCSR_9600SPS  = 5,
	ADCSR_4800SPS  = 6
#endif
}ADCsampleRate_t;

/**
 * @brief Type of reference voltage
 */
typedef enum
{
	ADCVR_AREF = (0 << REFS0),				/*!< Ref. voltage  AREF*/
	ADCVR_AVCC = (1 << REFS0),				/*!< Power voltage */
	ADCVR_INT  = (3 << REFS0)				/*!< Internal ref. voltage 2,56 V */
}ADCvref_t;

/**
 * @brief ADC channels
 */
typedef enum
{
	// Single channels
	ADCC_CH0          = 0x00,				/*!< Channel 0 */
	ADCC_CH1          = 0x01,				/*!< Channel 1 */
	ADCC_CH2          = 0x02,				/*!< Channel 2 */
	ADCC_CH3          = 0x03,				/*!< Channel 3 */
	ADCC_CH4          = 0x04,				/*!< Channel 4 */
	ADCC_CH5          = 0x05,				/*!< Channel 5 */
	ADCC_CH6          = 0x06,				/*!< Channel 6 */
	ADCC_CH7          = 0x07,				/*!< Channel 7 */

	// Differential channel
	// Amplification x1
	ADCC_CH0_CH1_X1   = 0x10,				/*!< Channel 0 +, Channel 1 - */
	ADCC_CH1_CH1_X1   = 0x11,				/*!< Channel 1 +, Channel 1 - */
	ADCC_CH2_CH1_X1   = 0x12,				/*!< Channel 2 +, Channel 1 - */
	ADCC_CH3_CH1_X1   = 0x13,				/*!< Channel 3 +, Channel 1 - */
	ADCC_CH4_CH1_X1   = 0x14,				/*!< Channel 4 +, Channel 1 - */
	ADCC_CH5_CH1_X1   = 0x15,				/*!< Channel 5 +, Channel 1 - */
	ADCC_CH6_CH1_X1   = 0x16,				/*!< Channel 6 +, Channel 1 - */
	ADCC_CH7_CH1_X1   = 0x17,				/*!< Channel 7 +, Channel 1 - */
	ADCC_CH0_CH2_X1   = 0x18,				/*!< Channel 0 +, Channel 2 - */
	ADCC_CH1_CH2_X1   = 0x19,				/*!< Channel 1 +, Channel 2 - */
	ADCC_CH2_CH2_X1   = 0x1A,				/*!< Channel 2 +, Channel 2 - */
	ADCC_CH3_CH2_X1   = 0x1B,				/*!< Channel 3 +, Channel 2 - */
	ADCC_CH4_CH2_X1   = 0x1C,				/*!< Channel 4 +, Channel 2 - */
	ADCC_CH5_CH2_X1   = 0x1D,				/*!< Channel 5 +, Channel 2 - */
	// Amplification x10
	ADCC_CH0_CH0_X10  = 0x08,				/*!< Channel 0 +, Channel 0 - */
	ADCC_CH1_CH0_X10  = 0x09,				/*!< Channel 1 +, Channel 0 - */
	ADCC_CH2_CH2_X10  = 0x0C,				/*!< Channel 2 +, Channel 2 - */
	ADCC_CH3_CH2_X10  = 0x0D,				/*!< Channel 3 +, Channel 2 - */
	// Amplification x200
	ADCC_CH0_CH0_X200 = 0x0A,				/*!< Channel 0 +, Channel 0 - */
	ADCC_CH1_CH0_X200 = 0x0B,				/*!< Channel 1 +, Channel 0 - */
	ADCC_CH2_CH2_X200 = 0x0E,				/*!< Channel 2 +, Channel 2 - */
	ADCC_CH3_CH2_X200 = 0x0F,				/*!< Channel 3 +, Channel 2 - */

	// Other channels
	ADCC_1_22V		  = 0x1E,				/*!< Voltage measurement 1.22V */
	ADCC_GND		  = 0x1F,				/*!< Voltage measurement 0V */
}ADCChannel_t;

/**
 * @brief Structure describing ADC
 */
typedef struct
{
	ADCmode_t Mode;							/*!< Measurement mode */
	ADCsampleRate_t SampleRate;				/*!< Sampling rate */
	ADCresolution_t Resolution;				/*!< Measurement resolution */
	ADCvref_t Vref;							/*!< Voltage reference */
	ADCChannel_t *ChannelList;				/*!< List of measured channels */
	uint8_t ChannelAmount;					/*!< Count of measured channels */
	volatile uint16_t *Buffer;				/*!< ADC value buffer */
	volatile uint32_t *OVSbuffer;			/*!< Oversampling buffer */
	/*! Measurement finished callback (of all channels) */
	void (*OnCompleted)();
}ADC_t;

// --->Functions

/*----------------------------------------------------------------------------*/
/**
 * @brief    Initializes the ADC.
 * @param    *adcConfig : configuration structure pointer
 * @retval   None
 */
void ADC_Init(ADC_t *adcConfig);

/*----------------------------------------------------------------------------*/
/**
 * @brief    Deinitializes pADC
 * @param    None
 * @retval   None
 */
void ADC_Deinit(void);

/*----------------------------------------------------------------------------*/
/**
 * @brief    Starts the measurement.
 * @param    None
 * @retval   None
 */
void ADC_StartConv(void);

/*----------------------------------------------------------------------------*/
/**
 * @brief    Stops the measurement.
 * @param    None
 * @retval   None
 */
void ADC_StopConv(void);

#endif								/* ADC_H_ */

/******************* (C) COPYRIGHT 2011 HENIUS ************** END OF FILE *****/
