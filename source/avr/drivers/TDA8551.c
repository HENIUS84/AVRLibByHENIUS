/**
 *******************************************************************************
 * @file     TDA8551.c
 * @author   HENIUS (Paweł Witak)
 * @version  1.1.1
 * @date     30-03-2012
 * @brief    Driver of audio amplifier TDA8551
 *******************************************************************************
 *
 * <h2><center>COPYRIGHT 2012 HENIUS</center></h2>
 */

/* Include section -----------------------------------------------------------*/

// --->System files

#include <math.h>

// --->User files

#include "TDA8551.h"

/* Variable section ----------------------------------------------------------*/

static uint8_t VolumeLevel;					/*!< Current volume */

/* Function section ----------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
void TDA8551_Init(void)
{
	TDA8551_MUTE();
	TDA8551_SetVolume(TDA8551_DEF_VOLUME);
	TDA8551_OPERATING();
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Single volume tick
 * @param    isLow : true - high state (false - low state)
 * @retval   None
 */
static void TDA8551_VolumeTick(bool isLow)
{
	if (isLow)
	{
		VOLUME_LOW();
	}
	else
	{
		VOLUME_HIGH();	
	}
	
	VOLUME_HIZ();			
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Saves volume level
 * @param    volume : volume level
 * @retval   Is limit rached?
 */
static bool TDA8551_SaveVolume(uint8_t volume)
{
	bool maxOrMin = false;
	
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
void TDA8551_SetVolume(uint8_t volume)
{
	uint8_t idx;
	
	TDA8551_SaveVolume(volume);
	
	// Set volume to 0
	for (idx = 0; idx < TDA8551_STEPS_COUNT; idx++)
	{
		TDA8551_VolumeTick(true);
	}
	
	// Sets volume to given value
	for (idx = 0; idx < VolumeLevel; idx++)
	{
		TDA8551_VolumeTick(false);
	}
}

/*----------------------------------------------------------------------------*/
void TDA8551_VolumeUpDown(bool isUp)
{
	VolumeLevel = isUp ? VolumeLevel + 1 : VolumeLevel - 1;	
	if (!TDA8551_SaveVolume(VolumeLevel))
	{		
		TDA8551_VolumeTick(!isUp);
	}
}

/*----------------------------------------------------------------------------*/
uint8_t TDA8551_GetVolume(void)
{
	return VolumeLevel;
}

/******************* (C) COPYRIGHT 2012 HENIUS *************** END OF FILE ****/
