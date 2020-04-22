/**
 *******************************************************************************
 * @file     SerialPort.h
 * @author   HENIUS (Paweł Witak)
 * @version  1.01.003
 * @date     15/11/2013
 * @brief    Serial Port driver with IRQ support (header file)
 *******************************************************************************
 *
 * <h2><center>COPYRIGHT 2013 HENIUS</center></h2>
 */

#ifndef  SERIAL_PORT_H
#define  SERIAL_PORT_H

/* Include section -----------------------------------------------------------*/

// --->System files

#include <stdbool.h>
#include <stdint.h>

/* Macros, constants and definitions section ---------------------------------*/

//--->Constants

#define SP_TX_BUFF_SIZE (128)       /*!< Size of sending buffer */
#if (SP_TX_BUFF_SIZE & (SP_TX_BUFF_SIZE - 1))
#error Size of sending buffer should be power of 2 
#endif
#define SP_RX_BUFF_SIZE (128)       /*!< Size of receiving buffer */
#if (SP_RX_BUFF_SIZE & (SP_RX_BUFF_SIZE - 1))
#error Size of receiving buffer should be power of 2
#endif

/* Declaration section -------------------------------------------------------*/

// --->Constatns and macros

/*! Macro to check port existence */
#define IS_SP_EXIST(port)	(port < SP_NUMBER_OF_PORTS)

#ifdef UDR1
/*! uC has USART1 module */
#define USART1
#endif

// Flags and registers
#ifndef USART0_TX_vect
/*! IRQ vector  of USART transmission */
#define USART0_TX_IRQ		(USART_UDRE_vect)
#else
/*! IRQ vector of USART0 transmission */
#define USART0_TX_IRQ		(USART0_UDRE_vect)
#endif

#ifndef USART0_RX_vect
/*! IRQ vector of USART receiving */
#define USART0_RX_IRQ		(USART_RXC_vect)
#else
/*! IRQ vector of USART0 receiving */
#define USART0_RX_IRQ		(USART0_RX_vect)
#endif

#ifndef UDR0
/*! UDR register */
#define UDR_0				(UDR)
#else
/*! UDR0 register */
#define UDR_0				(UDR0)
#endif

#ifndef UCSR0A
/*! UCSRA register */
#define UCSRA_0				(UCSRA)
#else
/*! UCSR0A register */
#define UCSRA_0				(UCSR0A)
#endif

#ifndef UCSR0B
/*! UCSRB register */
#define UCSRB_0				(UCSRB)
#else
/*! UCSR0B register */
#define UCSRB_0				(UCSR0B)
#endif

#ifndef UCSR0C
/*! UCSRC register */
#define UCSRC_0				(UCSRC)
#else
/*! UCSR0C register */
#define UCSRC_0				(UCSR0C)
#endif

#ifndef UBRR0H
/*! UBRRH register */
#define UBRRH_0				(UBRRH)
#else
/*! UBRR0H register */
#define UBRRH_0				(UBRR0H)
#endif

#ifndef UBRR0L
/*! UBRRL register */
#define UBRRL_0				(UBRRL)
#else
/*! UBRR0L register */
#define UBRRL_0				(UBRR0L)
#endif

#ifndef FE0
/*! FE flag */
#define FE_0				(FE)
#else
/*! FE0 flag */
#define FE_0				(FE0)
#endif

#ifndef DOR0
/*! DOR flag */
#define DOR_0				(DOR)
#else
/*! DOR0 flag */
#define DOR_0				(DOR0)
#endif

#ifndef UPE0
/*! UPE flag */
#define UPE_0				(PE)
#else
/*! UPE0 flag */
#define UPE_0				(UPE0)
#endif

#ifndef UCSZ00
/*! UCSZ0 flag */
#define UCSZ0_0				(UCSZ0)
#else
/*! UCSZ00 flag */
#define UCSZ0_0				(UCSZ00)
#endif

#ifndef UCSZ01
/*! UCSZ1 flag */
#define UCSZ1_0				(UCSZ1)
#else
/*! UCSZ01 flag */
#define UCSZ1_0				(UCSZ01)
#endif

#ifndef UCSZ02
/*! UCSZ2 flag */
#define UCSZ2_0				(UCSZ2)
#else
/*! UCSZ02 flag */
#define UCSZ2_0				(UCSZ02)
#endif

#ifndef USBS0
/*! USBS flag */
#define USBS_0				(USBS)
#else
/*! USBS0 flag */
#define USBS_0				(USBS0)
#endif

#ifndef UPM00
/*! UPM0 flag */
#define UPM0_0				(UPM0)
#else
/*! UPM00 flag */
#define UPM0_0				(UPM00)
#endif

#ifndef UMSEL0
/*! UMSEL flag */
#define UMSEL_0				(UMSEL)
#else
/*! UMSEL0 flag */
#define UMSEL_0				(UMSEL0)
#endif

#ifndef UCPOL0
/*! UCPOL flag */
#define UCPOL_0				(UCPOL)
#else
/*! UCPOL0 flag */
#define UCPOL_0				(UCPOL0)
#endif

#ifndef U2X0
/*! U2X flag */
#define U2X_0				(U2X)
#else
/*! U2X0 flag */
#define U2X_0				(U2X0)
#endif

#ifndef RXEN0
/*!< RXEN flag */
#define RXEN_0				(RXEN)
#else
/*! RXEN0 flag */
#define RXEN_0				(RXEN0)
#endif

#ifndef TXEN0
/*! TXEN flag */
#define TXEN_0				(TXEN)
#else
/*! TXEN0 flag */
#define TXEN_0				(TXEN0)
#endif

#ifndef TXCIE0
/*! TXCIE flag */
#define TXCIE_0				(TXCIE)
#else
/*! TXCIE0 flag */
#define TXCIE_0				(TXCIE0)
#endif

#ifndef RXCIE0
/*! RXCIE flag */
#define RXCIE_0				(RXCIE)
#else
/*! RXCIE0 flag */
#define RXCIE_0				(RXCIE0)
#endif

#ifndef RXC0
/*! RXC flag */
#define RXC_0				(RXC)
#else
/*! RXC0 flag */
#define RXC_0				(RXC0)
#endif

#ifndef UDRE0
/*! UDRE flag */
#define UDRE_0				(UDRE)
#else
/*! UDRE0 flag */
#define UDRE_0				(UDRE0)
#endif

#ifndef UDRIE0
/*! UDRIE flag */
#define UDRIE_0				(UDRIE)
#else
/*! UDRIE0 flag */
#define UDRIE_0				(UDRIE0)
#endif

// --->Types

/**
 * @brief Names of Serial Port
 */
typedef enum 
{
   SPN_USART0, 						/*!< USART0 port */
#ifdef UDR1
   SPN_USART1,						/*!< USART1 port */
#endif
   SP_NUMBER_OF_PORTS				/*!< Number of ports */
}ESPName_t;

/**
 * @brief Initialization structure of serial port
 */
typedef struct
{
	void(*Delay)(uint16_t time);	/*!< Pointer to delay function */
	uint32_t CpuFrequency;			/*!< CPU frequency in MHz  */
	bool IsPrintfEnabled;			/*!< printf activation flag */
	ESPName_t PrintfPort;			/*!< Serial Port name for printf */
}SPController_t;

/**
 * @brief Parity types
 */
typedef enum 
{
   SPP_NO_PARITY = 0, 				/*!< No parity */
   SPP_EVEN      = 2, 				/*!< Even */
   SPP_ODD       = 3				/*!< Odd */
}SPParity_t;

/**
 * @brief Type of signal slope
 */
typedef enum  
{
   SPE_TRRF,                        /*!< TxD - failing, RxD - raising */
   SPE_TFRR                         /*!< TxD - raising, RxD - failing */
}ESPEdge_t; 

/**
 * @brief Work mode (synchronous/asynchronous)
 */
typedef enum  
{
   SPSM_ASYNCHRONOUS, 				/*!< Asynchronous */
   SPSM_SYNCHRONOUS					/*!< Synchronous */
}ESPSyncMode_t;

/**
 * @brief Speed types (normal/double)
 */
typedef enum  
{
   SPSM_NORMAL, 					/*!< Normal speed */
   SPSM_DOUBLE						/*!< Double speed */
}ESPSpeedMode_t; 

/**
 * @brief Read status
 */
typedef enum
{
	SPRS_OK,						/*!< No issues */
	SPRS_PARITY_ERROR,				/*!< Parity error */
	SPRS_DATA_OVERRUN_ERROR,		/*!< Data overrun */
	SPRS_FRAME_ERROR, 				/*!< Frame error */
	SPRS_TIMEOUT_ERROR				/*!< Timeout error */
}ESPRcvStatus_t;

/**
 * @brief Transmission speed
 */
typedef enum
{
	
	SPBR_4800   = 4800,				/*!< 4800 b/s */	
	SPBR_9600   = 9600,				/*!< 9600 b/s */	
	SPBR_19200  = 19200,			/*!< 19200 b/s */	
	SPBR_38400  = 38400,			/*!< 38400 b/s */	
	SPBR_57600  = 57600,			/*!< 57600 b/s */	
	SPBR_115200 = 115200			/*!< 115200 b/s */	
}ESPBaudRate_t;

/**
 * @brief Serial Port description structure
 */
typedef struct
{
   ESPBaudRate_t BaudRate;          /*!< Speed rate in b/s */
   uint8_t DataLength;				/*!< Data length in Bytes (5-8) */
   uint8_t StopBits;                /*!< Stop bits (1 or 2) */
   SPParity_t Parity;               /*!< Parity type */   
   ESPEdge_t Edge;					/*!< TSignal edge (failing/raising) */               
   ESPSyncMode_t SyncMode;          /*!< Work mode (synchronous/asynchronous) */
   ESPSpeedMode_t SpeedMode;        /*!< Speed mode */
   ESPRcvStatus_t ReceiveStatus;	/*!< Receive status */
   bool IsIrqEnabled;				/*!< IRQ activation flag */
}SPDescriptor_t;

/**
 * @brief Serial Port structure
 */
typedef struct  
{
	/*! Receive buffer */
	uint8_t RxBuffer[SP_RX_BUFF_SIZE];	
	/*! Transmit buffer */
	uint8_t TxBuffer[SP_TX_BUFF_SIZE];
	uint8_t RxHead;					/*!< Head of receive buffer */
	uint8_t RxTail;					/*!< Tail of receive buffer */
	uint8_t TxHead;					/*!< Head of transmit buffer */
	uint8_t TxTail;					/*!< Tail of transmit buffer */
	struct
	{
		volatile uint8_t *rUCSRA;	/*!< UCSRA control register */
		volatile uint8_t *rUCSRB;	/*!< UCSRB control register */
		volatile uint8_t *rUCSRC;	/*!< UCSRC control register */
		volatile uint8_t *rUDR;		/*!< UDR data register */
		volatile uint8_t *rUBRRL;	/*!< UBRRL data register */
		volatile uint8_t *rUBRRH;	/*!< UBRRH data register */
	}Register;						/*!< Registers */
	struct 
	{
		uint8_t bRXC;				/*!< RXC bit */
		uint8_t bRXCIE;				/*!< RXCIE bit */
		uint8_t bTXCIE;				/*!< TXCIE bit */
		uint8_t bUCSZ0;				/*!< UCSZ0 bit */	
		uint8_t bUCSZ2;				/*!< UCSZ2 bit */
		uint8_t bUPM0;				/*!< UPM0 bit */
		uint8_t bUSBS;				/*!< USBS bit */	
		uint8_t bUMSEL;				/*!< UMSE bitL */	
		uint8_t bUCPOL;				/*!< UCPOL bit */
		uint8_t bU2X;				/*!< U2X bit */		
		uint8_t bRXEN;				/*!< RXEN bit */	
		uint8_t bTXEN;				/*!< TXEN bit */
		uint8_t bUDRE;				/*!< UDRE bit */
		uint8_t bUDRIE;				/*!< UDRIE bit */
	}Bit;							/*!< Bits */
	SPDescriptor_t *UsartDescriptor;/*!< Port descriptor (pointer) */
	bool IsPortOpen; 				/*!< Flag for open port */
	uint8_t ReceivedDataLength;		/*!< Receive data length */
}SerialPort_t;

// --->Functions

/*----------------------------------------------------------------------------*/
/**
 * @brief    Initializes serial port
 * @param    data : pointer to the initialization data
 * @retval   None
 */
void SerialPort_Init(SPController_t *data);

/*----------------------------------------------------------------------------*/
/**
 * @brief    OPens serial port
 * @param    serialPortName : serial port name
 * @param    serialPortConfig : port configuration
 * @retval   None
 */
void SerialPort_Open(ESPName_t serialPortName,
                     SPDescriptor_t *serialPortConfig);

/*----------------------------------------------------------------------------*/
/**
 * @brief    Close serial port
 * @param    serialPortName : serial port name
 * @retval   None
 */
void SerialPort_Close(ESPName_t serialPortName);

/*----------------------------------------------------------------------------*/
/**
 * @brief    Sends character without IRQ
 * @param    serialPortName : serial port name
 * @param    character : character to send
 * @retval   None
 */
void SerialPort_SendChar(ESPName_t serialPortName,
		                 uint8_t character);

/*----------------------------------------------------------------------------*/
/**
 * @brief    Sends character with IRQ
 * @param    serialPortName : serial port name
 * @param    character : character to send
 * @retval   None
 */
void SerialPort_SendChar_Irq(ESPName_t serialPortName, uint8_t character);

/*----------------------------------------------------------------------------*/
/**
 * @brief    Gets data number in receive buffer
 * @param    serialPortName : serial port name
 * @retval   Number of bytes in buffer
 */
uint16_t SerialPort_CountOfBytes(ESPName_t serialPortName);

/*----------------------------------------------------------------------------*/
/**
 * @brief    Gets bytes in receive buffer
 * @param    serialPortName : serial port
 * @param    offset : buffer offset
 * @retval   Pointer to the data
 */
uint8_t* SerialPort_ReadBytes(ESPName_t serialPortName,
                              uint16_t offset);			 

/*----------------------------------------------------------------------------*/
/**
 * @brief    Receives character without IRQ
 * @param    serialPortName : serial port name
 * @param    timeout: receive timeout in ms
 * @retval   Receive character (-1 no data)
 */
int16_t SerialPort_ReceiveChar(ESPName_t serialPortName, uint16_t timeout);

/*----------------------------------------------------------------------------*/
/**
 * @brief    Receives character with IRQ
 * @param    serialPortName : serial port name
 * @param    timeout: receive timeout in ms
 * @retval   Receive character (-1 no data)
 */
int16_t SerialPort_ReceiveChar_Irq(ESPName_t serialPortName, uint16_t timeout);

/*----------------------------------------------------------------------------*/
/**
 * @brief    Transmits single character
 * @param    serialPortName : serial port name
 * @param    _char : data to send
 * @retval   None
 */
void SerialPort_TransmitChar(ESPName_t serialPortName, uint8_t _char);

/*----------------------------------------------------------------------------*/
/**
 * @brief    Transmits text
 * @param    serialPortName : serial port name
 * @param    text : pointer to the text
 * @retval   None
 */
void SerialPort_TransmitText(ESPName_t serialPortName, uint8_t* text);

#endif								/* SERIAL_PORT_H */

/******************* (C) COPYRIGHT 2013 HENIUS *************** END OF FILE ****/