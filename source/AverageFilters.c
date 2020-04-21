/**
 *******************************************************************************
 * @file     AverageFilters.c
 * @author   HENIUS (Paweł Witak)
 * @version  1.1.1
 * @date     23-06-2011
 * @brief    Average filter implementation
 *******************************************************************************
 *
 * <h2><center>COPYRIGHT 2011 HENIUS</center></h2>
 */

/* Include section -----------------------------------------------------------*/

// --->System files

#include <stdio.h>
#include <stdbool.h>
#include <math.h>

// --->User files

#include "AverageFilters.h"

/* Variable section ----------------------------------------------------------*/

/* Function section ----------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
int32_t AverageFilter1(int32_t currentSample, AvgFilter1_t *filter)
{
	int32_t result;
		
	result = (((currentSample << AVGF1_SF) >> filter->TimeConstant) + 
	           (filter->PreviousSample << AVGF1_SF) - 
	          ((filter->PreviousSample << AVGF1_SF) >> filter->TimeConstant));
	result >>= AVGF1_SF;
	filter->PreviousSample = currentSample;
			
	return result;
}

/*----------------------------------------------------------------------------*/
uint16_t AverageFilter2(uint16_t currentSample, AvgFilter2_t *filter)
{
    // Average calculation
	filter->PreviousSum = filter->PreviousSum + (int32_t)currentSample - 
	                      filter->Buffer[filter->Index];
						  
	// Inserting sample into buffer
	filter->Buffer[filter->Index++] = currentSample;		
	filter->Index %= AVG_FILTER2_SIZE;
  
	return filter->PreviousSum  >> AVG_FILTER_SCALE;					   
}

/******************* (C) COPYRIGHT 2011 HENIUS *************** END OF FILE ****/
