/**
 *******************************************************************************
 * @file     AverageFilters.h
 * @author   HENIUS (Paweł Witak)
 * @version  1.01.001
 * @date     23-06-2011
 * @brief    Average filter implementation (header file)
 *******************************************************************************
 *
 * <h2><center>COPYRIGHT 2011 HENIUS</center></h2>
 */

#ifndef  AVERAGE_FILTERS_H_
#define  AVERAGE_FILTERS_H_

/* Include section -----------------------------------------------------------*/

// --->System files

#include <stdint.h>
#include <stdlib.h>

/* Macros, constants and definitions section ---------------------------------*/

// --->Constants

#define AVG_FILTER_SCALE	(6)				/*!< Factor of average filter 2 */
/*! Size of filter buffer 2 */
#define AVG_FILTER2_SIZE	(1 << AVG_FILTER_SCALE)		
#define AVGF1_SF			(8)				/*!< Scaling factor of filter 1 */
#define MF_SIZE				(5)

// --->Types

/**
 * @brief Configuration data for filter 1
 */
typedef struct  
{
	uint8_t TimeConstant;					/*!< Time constant */
	int32_t PreviousSample;					/*!< Previous sample */
}AvgFilter1_t;

/**
 * @brief Configuration data for filter 2
 */
typedef struct  
{
	uint8_t Index;							/*!< Sample index */	
	int32_t PreviousSum;					/*!< Previous sample sum */	
	int16_t Buffer[AVG_FILTER2_SIZE];		/*!< Sample buffer */				
}AvgFilter2_t;

/* Declaration section -------------------------------------------------------*/

// --->Functions

/*----------------------------------------------------------------------------*/
/**
 * @brief    Average filter #1
 * @param    currentSample : current sample
 * @param    *filter : pointe to the filter configuration structure
 * @retval   Average of all samples
 */
int32_t AverageFilter1(int32_t currentSample, AvgFilter1_t *filter);

/*----------------------------------------------------------------------------*/
/**
 * @brief    Average filter #2
 * @param    currentSample : current sample
 * @param    *filter : pointe to the filter configuration structure
 * @retval   Average of all samples
 */
uint16_t AverageFilter2(uint16_t currentSample, AvgFilter2_t *filter);

#endif								/* AVERAGE_FILTERS_H_ */

/******************* (C) COPYRIGHT 2011 HENIUS *************** END OF FILE ****/
