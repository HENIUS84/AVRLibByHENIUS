/**
 *******************************************************************************
 * @file     KS0108.c
 * @author   HENIUS (Paweł Witak)
 * @version  1.1.3
 * @date     12/04/2014
 * @brief    Driver of LCD display based on KS0108
 *******************************************************************************
 *
 * <h2><center>COPYRIGHT 2014 HENIUS</center></h2>
 */

/* Include section -----------------------------------------------------------*/

// --->System files

#include <stdint.h>
#include <avr/pgmspace.h>

// --->User files

#include "KS0108LCD.h"

/* Variable section ----------------------------------------------------------*/

static uint8_t LCD_x = 0;					/*!< X coordinate */
static uint8_t LCD_y = 0;               	/*!< Y coordinate */

#ifdef KS0108LCD_FRAME_BUFF_ENABLED
static bool IsScreenBlocked = false;				/*! Screen block flag */
/*! Display frame buffer*/
static uint8_t FrameBuffer[KS0108LCD_WIDTH][KS0108LCD_HEIGHT / 8];

#endif

/* Function section ----------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/**
 * @brief    Selects controller
 * @param    number : number of controller
 * @retval   None
 */
static void KS0108LCD_SelectController(uint8_t number)
{
	switch (number)
	{
		// --->Controller #1
		case 1:
			KS0108LCD_CS1_PORT |= KS0108LCD_CS1;
			KS0108LCD_CS2_PORT &= ~KS0108LCD_CS2;
			#ifdef KS0108LCD_CS3_PORT
			KS0108LCD_CS3_PORT &= ~KS0108LCD_CS3;
			#endif					// KS0108LCD_CS3_PORT
		
			break;
			
		// --->Controller #2
		case 2:
			KS0108LCD_CS1_PORT &= ~KS0108LCD_CS1;
			KS0108LCD_CS2_PORT |= KS0108LCD_CS2;
			#ifdef KS0108LCD_CS3_PORT
			KS0108LCD_CS3_PORT &= ~KS0108LCD_CS3;
			#endif					// KS0108LCD_CS3_PORT
		
			break;
		
		// --->Controller #3
		case 3:
			KS0108LCD_CS1_PORT &= ~KS0108LCD_CS1;
			KS0108LCD_CS2_PORT &= ~KS0108LCD_CS2;
			#ifdef KS0108LCD_CS3_PORT
			KS0108LCD_CS3_PORT |= KS0108LCD_CS3;
			#endif					// KS0108LCD_CS3_PORT
		
			break;
	}
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Turn of controllers
 * @param    None
 * @retval   None
 */
static void KS0108LCD_NoCS(void)
{
	KS0108LCD_CS1_PORT &= ~KS0108LCD_CS1;
	#ifdef KS0108LCD_CS2_PORT
	KS0108LCD_CS2_PORT &= ~KS0108LCD_CS2;
	#endif							// KS0108LCD_CS2_PORT
	#ifdef KS0108LCD_CS3_PORT
	KS0108LCD_CS3_PORT &= ~KS0108LCD_CS3;
	#endif							// KS0108LCD_CS3_PORT
}

/*----------------------------------------------------------------------------*/
void KS0108LCD_Init(void)
{
	// Ports initialization
	KS0108LCD_DATA_DDR = 0xFF;
	KS0108LCD_RS_DDR |= KS0108LCD_RS;
	KS0108LCD_RW_DDR |= KS0108LCD_RW;
	KS0108LCD_EN_DDR |= KS0108LCD_EN;
	KS0108LCD_RST_DDR |= KS0108LCD_RST;
	KS0108LCD_CS1_DDR |= KS0108LCD_CS1;
	KS0108LCD_CS2_DDR |= KS0108LCD_CS2;

	KS0108LCD_CS1_PORT |= KS0108LCD_CS1;
	KS0108LCD_CS2_PORT |= KS0108LCD_CS2;
	KS0108LCD_EN_PORT |= KS0108LCD_EN;
	KS0108LCD_RW_PORT |= KS0108LCD_RW;
	KS0108LCD_RS_PORT |= KS0108LCD_RS;
	KS0108LCD_RST_PORT |= KS0108LCD_RST;
   
	KS0108LCD_On();
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Waits for previous instruction
 * @param    None
 * @retval   None
 */
static void KS0108LCD_Wait(void)
{
	uint16_t timeoutTimer = 10;
	
	// Receive mode of display
	KS0108LCD_DATA_DDR = 0x00;             
	KS0108LCD_RS_PORT &= ~KS0108LCD_RS;          
	KS0108LCD_RW_PORT |= KS0108LCD_RW;            
	
	// BUSY flag check                                            
	do                                            
	{
		KS0108LCD_EN_PORT |= KS0108LCD_EN;  
		KS0108LCD_EN_PORT &= ~KS0108LCD_EN;
	}while ((KS0108LCD_DATA_PIN & KS0108LCD_BUSY) && --timeoutTimer);        
        
	// Sending mode of display                                        	
	KS0108LCD_DATA_DDR = 0xFF;
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Writes instruction to the controller
 * @param    cmd : instruction code
 * @retval   None
 */
static void KS0108LCD_WriteCmd(uint8_t cmd)
{
	KS0108LCD_Wait();
	KS0108LCD_RS_PORT &= ~KS0108LCD_RS;	
	KS0108LCD_RW_PORT &= ~KS0108LCD_RW; 
	KS0108LCD_DATA_DDR |= 0xFF; 
	KS0108LCD_DATA_PORT = cmd;
	KS0108LCD_STROBE();
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Writes data to the controller
 * @param    data : data byte
 * @retval   None
 */
static void KS0108LCD_WriteData(uint8_t data)
{  
	// Controller selection
	if (LCD_x < KS0108_SIZE)
	{
		KS0108LCD_SelectController(1);
	}
	else if (LCD_x >= KS0108_SIZE && LCD_x < (KS0108_SIZE * 2))
	{
		KS0108LCD_SelectController(2);
	}
	else
	{
		KS0108LCD_SelectController(3);
	}
	
	KS0108LCD_Wait();
	KS0108LCD_RS_PORT |= KS0108LCD_RS;    // Data mode
	KS0108LCD_RW_PORT &= ~KS0108LCD_RW;   // Write mode
	KS0108LCD_DATA_PORT = data;  
	KS0108LCD_STROBE();
	LCD_x++;
	
	// End of screen?
	if (LCD_x > KS0108LCD_WIDTH)
	{
		LCD_x = 0;
	}
	KS0108LCD_NoCS();
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Reads data from controller
 * @param    None
 * @retval   Received byte
 */
static uint8_t KS0108LCD_ReadData(void)
{
	uint8_t data = 0;

	// Controller selection
	if (LCD_x < KS0108_SIZE)
	{
		KS0108LCD_SelectController(1);
	}
	else if (LCD_x >= KS0108_SIZE && LCD_x < (KS0108_SIZE * 2))
	{
		KS0108LCD_SelectController(2);
	}
	else
	{
		KS0108LCD_SelectController(3);
	}
	
	KS0108LCD_Wait();
	KS0108LCD_RS_PORT |= KS0108LCD_RS;	// Data mode   
	KS0108LCD_RW_PORT |= KS0108LCD_RW;	// Read mode
	KS0108LCD_EN_PORT |= KS0108LCD_EN;
	KS0108LCD_EN_PORT |= KS0108LCD_EN; 
	asm("nop");
	KS0108LCD_DATA_DDR = 0x00;
	KS0108LCD_DATA_PORT = 0xFF;
	data = KS0108LCD_DATA_PIN;
	KS0108LCD_EN_PORT &= ~KS0108LCD_EN;
	LCD_x++;
	
	// End of screen?
	if (LCD_x > (KS0108LCD_WIDTH - 1))   
	{
		LCD_x = 0;
	}
	KS0108LCD_NoCS();
	KS0108LCD_DATA_DDR = 0xFF;

	return data;
}

/*----------------------------------------------------------------------------*/
void KS0108LCD_GoTo(uint16_t x, uint16_t y)
{  
	// Address mode
	KS0108LCD_RS_PORT &= ~KS0108LCD_RS;	
	KS0108LCD_RW_PORT &= ~KS0108LCD_RW;	
   
	LCD_x = x;
	LCD_y = y;
   
	// Controller selection
	if (LCD_x >= KS0108_SIZE && LCD_x < (KS0108_SIZE * 2)) 
	{
		KS0108LCD_SelectController(2);		            
		KS0108LCD_WriteCmd(KS0108LCD_SET_Y | LCD_y); 
		KS0108LCD_WriteCmd(KS0108LCD_SET_X | (LCD_x - KS0108_SIZE));  
	}
	else if (LCD_x >= (KS0108_SIZE * 2))
	{
		KS0108LCD_SelectController(3);
		KS0108LCD_WriteCmd(KS0108LCD_SET_Y | LCD_y);
		KS0108LCD_WriteCmd(KS0108LCD_SET_X | (LCD_x - (KS0108_SIZE * 2)));
	}
	else                            
	{
		KS0108LCD_SelectController(1);
		KS0108LCD_WriteCmd(KS0108LCD_SET_Y | LCD_y);
		KS0108LCD_WriteCmd(KS0108LCD_SET_X | LCD_x); 
		KS0108LCD_SelectController(2);
		KS0108LCD_WriteCmd(KS0108LCD_SET_Y | LCD_y);
		KS0108LCD_WriteCmd(KS0108LCD_SET_X | 0 );     
	}
   
	KS0108LCD_SelectController(1);
	KS0108LCD_WriteCmd(KS0108LCD_START_LINE | 0); 
	KS0108LCD_SelectController(2);
	KS0108LCD_WriteCmd(KS0108LCD_START_LINE | 0);
	KS0108LCD_NoCS();
}

/*----------------------------------------------------------------------------*/
void KS0108LCD_On(void)
{
	KS0108LCD_SelectController(1);
	KS0108LCD_WriteCmd(KS0108LCD_STATE | KS0108LCD_ON); 
	KS0108LCD_SelectController(2);
	KS0108LCD_WriteCmd(KS0108LCD_STATE | KS0108LCD_ON);
	KS0108LCD_WriteCmd(KS0108LCD_STATE | KS0108LCD_ON);
	KS0108LCD_NoCS();
}

/*----------------------------------------------------------------------------*/
/**
 * @brief    Turns of the display
 * @param    None
 * @retval   None
 */
void KS0108LCD_Off(void)
{
   KS0108LCD_SelectController(1);
   KS0108LCD_WriteCmd(KS0108LCD_STATE | KS0108LCD_OFF);
   KS0108LCD_WriteCmd(KS0108LCD_STATE | KS0108LCD_OFF);
   KS0108LCD_WriteCmd(KS0108LCD_STATE | KS0108LCD_OFF);
   KS0108LCD_NoCS();
}

/*----------------------------------------------------------------------------*/
void KS0108LCD_BlockScreen(bool isBlocked) 
{
	#ifdef KS0108LCD_FRAME_BUFF_ENABLED
	
	IsScreenBlocked = isBlocked;
	
	#endif
}

/*----------------------------------------------------------------------------*/
void KS0108LCD_Clear(uint8_t pattern)
{
	uint16_t x, y;
	
	#ifdef KS0108LCD_FRAME_BUFF_ENABLED
	
	for (y = 0; y < (KS0108LCD_HEIGHT / 8); y++)          
	{
		for (x = 0; x < KS0108LCD_WIDTH; x++) 
		{   
			FrameBuffer[x][y] = pattern; 
		}
	}
	
	#else	  
	for (y = 0; y < (KS0108LCD_HEIGHT / 8); y++)          
	{
		KS0108LCD_GoTo(0, y);
		
		for (x = 0; x < KS0108LCD_WIDTH; x++) 
		{  
			KS0108LCD_WriteData(pattern); 
		}
	}
	KS0108LCD_GoTo(0, 0);
	
	#endif
}

/*----------------------------------------------------------------------------*/
void KS0108LCD_MemoryHandler(void)
{
#ifdef KS0108LCD_FRAME_BUFF_ENABLED
	static uint16_t y = 0;
	uint16_t x;
	
	if (!IsScreenBlocked)
	{		
		KS0108LCD_GoTo(0, y);
		
		for (x = 0; x < KS0108LCD_WIDTH; x++) 
		{   
			KS0108LCD_WriteData(FrameBuffer[x][y]);
		}
	
		y++;
		y %= KS0108LCD_HEIGHT / 8;
	}
#endif	
}

/*----------------------------------------------------------------------------*/
void KS0108LCD_PutImage(uint8_t *image,
                        int16_t offsetX, int16_t offsetY, 
				        bool isTransparentBackground)
{
	uint16_t i, j;
	int16_t x, y;
	
#ifdef KS0108LCD_FRAME_BUFF_ENABLED
	for (j = 0; j < KS0108LCD_HEIGHT / 8; j++) 
	{   
		for (i = 0; i < KS0108LCD_WIDTH; i++)
		{	
			x = i - offsetX;
			y = j - offsetY;
					
			if (x >= 0 && y >= 0 &&
			    x < KS0108LCD_WIDTH && y < KS0108LCD_HEIGHT / 8)
			{
				FrameBuffer[i][j] = image[y * KS0108LCD_WIDTH + x];
			}	
			else if (!isTransparentBackground)
			{
				FrameBuffer[i][j] = 0;				
			}		
		}		
	}
#else
	for (j = 0; j < KS0108LCD_HEIGHT / 8; j++) 
	{				
		y = j - offsetY;	
		KS0108LCD_GoTo(0, y);
		
		for (i = 0; i < KS0108LCD_WIDTH; i++) 
		{   			
			x = i - offsetX;
			
			if (x >= 0 && y >= 0 &&
			    x < KS0108LCD_WIDTH && y < KS0108LCD_HEIGHT / 8)
			{					   
				KS0108LCD_WriteData(image[y * KS0108LCD_WIDTH + x]);
			}			
		}
	}	
#endif
}

/*----------------------------------------------------------------------------*/
void KS0108LCD_PutImage_P(uint8_t *image, 
                          int16_t offsetX, int16_t offsetY,
					      bool isTransparentBackground)
{
	uint16_t i, j;
	int16_t x, y;
	
#ifdef KS0108LCD_FRAME_BUFF_ENABLED
	for (j = 0; j < KS0108LCD_HEIGHT / 8; j++) 
	{   
		for (i = 0; i < KS0108LCD_WIDTH; i++)
		{	
			x = i - offsetX;
			y = j - offsetY;
					
			if (x >= 0 && y >= 0 &&
			    x < KS0108LCD_WIDTH && y < KS0108LCD_HEIGHT / 8)
			{
				FrameBuffer[i][j] =  
					pgm_read_byte(&image[y * KS0108LCD_WIDTH + x]);
			}	
			else if (!isTransparentBackground)
			{
				FrameBuffer[i][j] = 0;				
			}		
		}		
	}
#else
	for (j = 0; j < KS0108LCD_HEIGHT / 8; j++) 
	{				
		y = j - offsetY;		
		KS0108LCD_GoTo(0, y);
		
		for (i = 0; i < KS0108LCD_WIDTH; i++) 
		{   			
			x = i - offsetX;
			
			if (x >= 0 && y >= 0 &&
			    x < KS0108LCD_WIDTH && y < KS0108LCD_HEIGHT / 8)
			{					   
				KS0108LCD_WriteData(pgm_read_byte(&image[y * KS0108LCD_WIDTH + x]));
			}			
		}
	}	
#endif
}

/*----------------------------------------------------------------------------*/
void KS0108LCD_SetPixel(uint16_t x, uint16_t y, uint16_t color)
{
#ifdef KS0108LCD_FRAME_BUFF_ENABLED
	
	if (color == KS0108LCD_BLACK_COLOR)
	{
		FrameBuffer[x][y / 8] |= (1 << (y % 8));
	}
	else if (KS0108LCD_WHITE_COLOR)
	{
		FrameBuffer[x][y / 8] &= ~(1 << (y % 8));
	}
	
#else
	
	uint8_t temp = 0;

	KS0108LCD_GoTo(x, y / 8);
	KS0108LCD_ReadData();
	temp = KS0108LCD_ReadData();
	KS0108LCD_GoTo(x, y / 8);

	if (color == KS0108LCD_BLACK_COLOR)
	{
		KS0108LCD_WriteData(temp | (1 << (y % 8)));
	}
	else if (KS0108LCD_WHITE_COLOR)
	{
		KS0108LCD_WriteData(temp & ~(1 << (y % 8)));
	}
	
	#endif
}

/******************* (C) COPYRIGHT 2014 HENIUS *************** END OF FILE ****/
