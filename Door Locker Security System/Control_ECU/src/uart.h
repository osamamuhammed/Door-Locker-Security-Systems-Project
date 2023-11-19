/******************************************************************************
 *
 * Module: UART
 *
 * File Name: uart.h
 *
 * Description: Header file for the UART AVR driver
 *
 * Author:  Osama M. Shehata
 *
 *******************************************************************************/

#ifndef UART_H_
#define UART_H_

#include "std_types.h"
/*******************************************************************************
 *                         Types Declaration                                   *
*******************************************************************************/

typedef enum {
	FIVE,SIX,SEVEN,EIGHT,NINE=0x07
} UART_BitData;

typedef enum {
	DISABLED,EVEN_PARITY=0X02,ODD_PARITY=0X03
} UART_Parity;

typedef enum {
	ONE_BIT,TWO_BIT
} UART_StopBit;

typedef enum {
	BAUDRATE_10,BAUDRATE_300,BAUDRATE_600,BAUDRATE_1200,BAUDRATE_2400,BAUDRATE_4800,BAUDRATE_9600
	,BAUDRATE_14400,BAUDRATE_19200,BAUDRATE_38400,BAUDRATE_57600,BAUDRATE_115200,BAUDRATE_128000,BAUDRATE_256000
} UART_BaudRate;

typedef struct {
	UART_BitData bit_data;
	UART_Parity parity;
	UART_StopBit stopbit;
	UART_BaudRate baudrate;
} UART_ConfigType;

/*******************************************************************************
 *                      Functions Prototypes                                   *
********************************************************************************/

/*
 * Description :
 * Functional responsible for Initialize the UART device by:
 * 1. Setup the Frame format like number of data bits, parity bit type and number of stop bits.
 * 2. Enable the UART.
 * 3. Setup the UART baud rate.
 */
void UART_init(const UART_ConfigType * Config_Ptr);

/*
 * Description :
 * Functional responsible for send byte to another UART device.
 */
void UART_sendByte(const uint8 data);

/*
 * Description :
 * Functional responsible for receive byte from another UART device.
 */
uint8 UART_recieveByte(void);

/*
 * Description :
 * Send the required string through UART to the other UART device.
 */
void UART_sendString(const uint8 *Str);

/*
 * Description :
 * Receive the required string until the '#' symbol through UART from the other UART device.
 */
void UART_receiveString(uint8 *Str); // Receive until #

#endif /* UART_H_ */
