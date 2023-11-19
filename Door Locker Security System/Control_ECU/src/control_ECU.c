/*
 ================================================================================================
 Name        : Ctrl_ECU.c
 Author      : Osama M. Shehata
 Description : This MC is responsible for processing data and taking actions such as password
 verification, Control alarm system and Control door motor.
 Date        : 16-8-2023
 ================================================================================================
 */
#include "Buzzer.h"
#include "external_eeprom.h"
#include "dc_motor.h"
#include "uart.h"
#include "std_types.h"
#include "avr/delay.h"
#include <avr/interrupt.h>
#include "timer1.h"
#include "gpio.h"
/*******************************************************************************
 *                           Global Variables                                  *
 *******************************************************************************/
unsigned char g_ticks = 0; /* Global variable that contains timer ticks count */

/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/
/* Description:
 * This function counts the ticks of the timer ( callback function)
 */
void TIMER1_Ticks(void) {
	g_ticks++;
}

/* Description:
 * This function is responsible for:
 * Receiving password from HMI_ECU
 * saving password in EEPROM
 */
void savePass(void) {
	uint8 byte_user, i, check1, check2, matched = 0;
	for (i = 0; i < 5; i++) {
		/* Receive password for first from HMI_ECU */
		byte_user = UART_recieveByte();
		EEPROM_writeByte((0x01) + i, byte_user);
		_delay_ms(500);

	}
	for (i = 6; i < 11; i++) {
		/* Receive password for second time from HMI_ECU */
		byte_user = UART_recieveByte();
		EEPROM_writeByte((0x01) + i, byte_user);
		_delay_ms(500);

	}
	/* Verify that two password matched*/
	for (i = 0; i < 5; i++) {
		EEPROM_readByte((0x01) + i, &check1);
		_delay_ms(10);
		EEPROM_readByte((0x07) + i, &check2);
		_delay_ms(10);
		if (check1 == check2) {
			matched++;
		}
	}
	if (matched == 5) {
		/* Send 1 to HMI_ECU to indicate the password verified*/
		UART_sendByte(1);
	} else {
		UART_sendByte(0);
	}
	matched = 0;
}

/* Description:
 * This function is responsible for:
 * Receiving password from HMI_ECU
 * Verify password
 * 1: Password matched
 * 0: Password is incorrect
 */
void passVerify(void) {
	uint8 byte_user, byte_memory, i, matched = 0;
	for (i = 6; i < 11; i++) {
		byte_user = UART_recieveByte(); /* Receive one byte from HMI_ECU */
		_delay_ms(500);
		EEPROM_readByte((0x01) + i, &byte_memory); /* Read one byte from EEPROM starting from 0x07*/
		_delay_ms(10);
		if (byte_memory == byte_user) {
			matched++; /* If both bytes matched so password is verified */
		}
	}
	if (matched == 5) {
		/* The password entered by the user matched the
		 saved password in EEPROM only if matched = 5 */
		/* Send 1 to HMI_ECU to indicate the password verified*/
		UART_sendByte(1);
	} else {
		/* Send 0 to HMI_ECU to indicate the password is not correct*/
		UART_sendByte(0);

	}
	/* Reset flag */
	matched = 0;
}

int main(void) {
	uint8 command; /* Variable that holds commands received from HMI ECU using UART*/

	/************************* UART Configuration *************************
	 * 8-bit data
	 * Parity bit is disabled
	 * ONE stop bit
	 * Baud-rate: 9600
	 */
	UART_ConfigType uart_config = { EIGHT, DISABLED, ONE_BIT, BAUDRATE_9600 };
	UART_init(&uart_config); /* Initialize UART with the required configuration */

	/************************* TIMER1 Configuration *************************
	 * Initial value : 0
	 * Compare value : 1000
	 * Pre-scalar: 256
	 * Mode: Compare
	 * Callback Function: TIMER1_Ticks()
	 */
	Timer1_ConfigType timer1_config = { 0, 999, F_CPU_256, COMPARE };
	Timer1_init(&timer1_config);
	Timer1_setCallBack(TIMER1_Ticks);

	/********* Global interrupts enable I-bit*******************/
	SREG |= (1 << 7);

	/*Initialize External drivers
	 * DC motors
	 * Buzzer
	 */
	DcMotor_Init();
	Buzzer_init();
	while (1) {
		command = UART_recieveByte();
		switch (command) {
		case '+':
			/* Create/Change password */
			savePass();
			break;

		case '@':
			/* Turn ON buzzer */
			/*Activate Buzzer for 60 seconds*/
			Buzzer_on();
			g_ticks = 0;
			while (g_ticks <61) {/* wait for 60 seconds */}
			Buzzer_off();
			break;

		case '%':
			/* Open the door */
			DcMotor_Rotate(CW, 100);
			g_ticks = 0;
			while (g_ticks <16) {/* wait for 15 seconds */}
			DcMotor_Rotate(STOP, 0);
			while (g_ticks <19) {/* Hold the door for 3 seconds */}
			DcMotor_Rotate(CCW, 100);
			while (g_ticks <34) {/* wait for 15 seconds */}
			DcMotor_Rotate(STOP, 0);
			break;

		case '#':
			/* Verify password */
			passVerify();
			break;
		}

	}
}

