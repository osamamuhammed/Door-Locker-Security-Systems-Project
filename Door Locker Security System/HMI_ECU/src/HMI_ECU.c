/*
 ================================================================================================
 Name        : Ctrl_Main.c
 Author      : Osama M. Shehata
 Description : This MC is responsible for processing data and taking actions such as password
 verification, Control alarm system and Control door motor.
 Date        : 16-8-2023
 ================================================================================================
 */

#include "lcd.h"
#include "keypad.h"
#include <util/delay.h> /* For the delay functions */
#include "timer1.h"
#include "uart.h"
#include<avr/interrupt.h>


/*******************************************************************************
 *                      Functions Prototypes                                   *
 ********************************************************************************/
void optionsMenu(void);
void PassVerify(void);
void buzzerON(void);
void openDoor(void);
void passInput(void);
void createPass(void);

/*******************************************************************************
 *                           Global Variables                                  *
 *******************************************************************************/
uint8 g_passCheck = 0; /* Global variable that contains a byte received from the Control ECU using UART */
unsigned char g_ticks = 0; /* Global variable that contains timer ticks count */
/*******************************************************************************
 *                     Functions Definitions                                   *
 *******************************************************************************/
/* Description:
 * This function counts the ticks of the timer ( callback function)
 */
void TIMER1_Ticks(void) {
	g_ticks++;
}
/* Description:
 * This function is responsible for displaying main options menu on LCD
 */
void optionsMenu(void) {
	/* The LCD will always display the main system option */
	LCD_clearScreen();
	LCD_displayStringRowColumn(0, 0, "+ : Open Door");
	LCD_displayStringRowColumn(1, 0, "- : Change Pass");

}

/* Description:
 * This function is responsible for reading password user input and send it to control ECU to check it
 */
void PassVerify(void) {
	/******************* Step3 - Open Door + *****************/
	/*
	 * Display a message to ask for password
	 * Send a data byte '@' to Control ECU - a command to read password and verify it
	 * Take password from user and receive passCheck flag from control ECU
	 */

	/* Clear LCD and display the message "Plz enter pass" */
	LCD_clearScreen();
	LCD_displayStringRowColumn(0, 0, "Plz Enter Pass");
	LCD_moveCursor(1, 0); /* Move the cursor to the second row */
	UART_sendByte('#');
	passInput();

}

/* Description:
 * This function is responsible for turning on the buzzer
 */
void buzzerON(void) {
	/* Clear LCD and display the message "System Error!" */
	LCD_clearScreen();
	LCD_displayStringRowColumn(0, 0, "System Error!");
	/* Send a command that is responsible for turning ON the buzzer*/
	UART_sendByte('@');
	/* Reset the global variable of counts */
	g_ticks = 0;
	while (g_ticks < 61) {/*wait for 1 minute - 60 seconds*/}
}

/* Description:
 * This function is responsible for Opening the door for 15 seconds then holds
 * it for 3 seconds then close the door after 15 seconds
 */
void openDoor(void) {
	/*if two passwords are matched:
	 * rotates motor for 15-seconds CW and display a message on
	 the screen “Door is Unlocking”
	 * hold the motor for 3-seconds.
	 * Rotates motor for 15-seconds A-CW and display a message on
	 the screen “Door is Locking”*/

	/* Send a data byte 'M' to Control ECU to control motor*/

	/* Reset the global variable of counts */
	g_ticks = 0;
	LCD_clearScreen();
	LCD_displayStringRowColumn(0, 0, "Door is ");
	LCD_displayStringRowColumn(1, 0, "Unlocking");
	UART_sendByte('%');
	while (g_ticks < 16) {
		/* waits for 15 seconds*/
	}
	LCD_clearScreen();
	LCD_displayStringRowColumn(0, 0, "Closing in 3s");
	while (g_ticks < 19) {
		/* waits for 3 seconds*/
	}
	LCD_clearScreen();
	LCD_displayStringRowColumn(0, 0, "Door is ");
	LCD_displayStringRowColumn(1, 0, "Locking");

	while (g_ticks < 34) {
		/* waits for 15 seconds*/
	}
	/* Reset the global variable of counts */
	g_ticks = 0;

}

/* Description:
 * This function is responsible for password verification
 */
void passInput(void) {
	uint8 key, i;
	for (i = 0; i < 5; i++) {
		/* Get the pressed key number, if any switch pressed for more than 500 ms
		 * it will considered more than one press */
		key = KEYPAD_getPressedKey();

		if ((key <= 9) && (key >= 0)) {
			/* Displays * when any number is pressed */
			LCD_displayCharacter('*');
			/* send data to Control_ECU using UART */
			UART_sendByte(key);
		} else {
			i--; /* Password consists of 5 numbers and can not include characters */
		}
		_delay_ms(500); /* Press time */
	}
	key = KEYPAD_getPressedKey();
	if (key == '=') /* '=' button is used as enter button */
	{
		/*passCheck byte is used to detect if password matched or not
		 * 0: password did not match
		 * 1: password matched successfully
		 * */
		g_passCheck = UART_recieveByte();
		_delay_ms(500); /* Delay to receive one byte from UART */
	}
}

/* Description:
 * This function is responsible for creating password or changing it
 */
void createPass(void) {
	/* Clear LCD and display the message "Plz enter pass" */
	LCD_clearScreen();
	LCD_displayStringRowColumn(0, 0, "Plz Enter Pass");
	LCD_moveCursor(1, 0); /* Move the cursor to the second row */

	/*Send a data byte '@' to Control ECU to make it ready to read and save password*/
	UART_sendByte('+');
	uint8 key, i;
	for (i = 0; i < 5; i++) {

		/* Get the pressed key number, if any switch pressed for more than 500 ms it will considered more than one press */
		key = KEYPAD_getPressedKey();

		if ((key <= 9) && (key >= 0)) {
			/* Displays * when any number is pressed */
			LCD_displayCharacter('*');
			/* send data to Control_ECU using UART */
			UART_sendByte(key);
		} else {
			i--; /* Password consists of 5 numbers and can not include characters */
		}
		_delay_ms(500); /* Press time */
	}

	key = KEYPAD_getPressedKey();
	if (key == '=') /* '=' button is used as enter button */
	{
		/*Ask the user to renter the same password for confirmation*/
		LCD_clearScreen();
		LCD_displayStringRowColumn(0, 0, "Plz re-enter the"); /*Displays this message “Plz re-enter the same Pass:*/
		LCD_displayStringRowColumn(1, 0, "same pass:");
	}
	LCD_moveCursor(1, 10); /* Move the cursor to the second row */

	for (i = 0; i < 5; i++) {
		/* Get the pressed key number, if any switch pressed for more than 500 ms it will considered more than one press */
		key = KEYPAD_getPressedKey();
		if ((key <= 9) && (key >= 0)) {

			LCD_displayCharacter('*'); /* Displays * when any number is pressed */
			UART_sendByte(key); /* send data to Control_ECU using UART */

		} else {

			i--; /*Password consists of 5 numbers and can not include characters*/
		}
		_delay_ms(500); /* Press time */
	}

	key = KEYPAD_getPressedKey();

	if (key == '=')/* '=' button is used as enter button */
	{
		/*passCheck byte is used to detect if password matched or not
		 * 0: password did not match
		 * 1: password matched successfully
		 * */
		g_passCheck = UART_recieveByte();
		_delay_ms(500); /* Delay to receive one byte from UART */
	}
}
/*******************************************************************************
 *                   The MAIN of the program                                   *
 *******************************************************************************/
int main(void) {
	uint8 key_main;
	/************************* LCD initialization **************************/
	LCD_init(); /*Initialize the LCD*/

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
	SREG |=(1<<7);
	/**********************************************************/
	/************ Step1 – Create a System Password ************/
	/**********************************************************/

	createPass();

	/*If the two passwords are matched then the system
	 has a password now and save it inside the EEPORM and go to Step 2.*/

	if (g_passCheck != 1) { /*Checks if two passwords are matched or not */
		/*If the two passwords do not match then repeat step 1 again*/
		createPass();

		if (g_passCheck != 1) {
			/* If the two passwords do not match for the second time
			 then show an ERROR and close the system*/
			LCD_clearScreen();
			LCD_displayStringRowColumn(0, 0, "Password does");
			LCD_displayStringRowColumn(1, 0, "not match");
			return 0;
		}
	}

	while (1) {
		/**********************************************************/
		/******************* Step2 - Main Options *****************/
		/**********************************************************/
		optionsMenu();

		/* Checks if any button is pressed and holds its value */
		key_main = KEYPAD_getPressedKey();

		switch (key_main) {

		case '+':
			/**********************************************************/
			/******************* Step3 - Open Door + ******************/
			/**********************************************************/

			/* if user asks to open the door
			 * Ask the user to enter password
			 * Send a data byte '@' to Control ECU to make
			 it ready for pass verification
			 * Password verification
			 */
			PassVerify(); /* Step 3 */

			if (g_passCheck == 1) {
				openDoor();
			} else {
				/* If the two passwords did not match repeat step 3 */
				PassVerify(); /* Step 3 */

				if (g_passCheck == 1) {
					/* If passwords matched open the door */
					openDoor();
				} else {
					/* If Password did not match for the second time repeat step 3 again */

					PassVerify(); /* Step 3 */

					if (g_passCheck == 1) {
						/*If passwords matched open the door */
						openDoor();
					} else {
						/*If password did not match for the third consecutive time
						 * Turn on the buzzer
						 * Display error message on LCD for 1 minute.
						 * System should be locked no inputs from Keypad will be
						 accepted during this time period.
						 * Go to Step 2 the main options again.
						 */
						buzzerON();
					}
				}
			}
			break;
		case '-':
			/**********************************************************/
			/**************** Step 4 - Change Password - **************/
			/**********************************************************/
			/* To change password user have to enter the old password first
			 * Take password from user and verify it
			 * if verified: Start changing password process
			 * if not verified: ask user to enter the password again  */

			PassVerify();
			if (g_passCheck == 1) {
				createPass();
				/*If the two passwords are matched then the system
				 has a new password now and saved it inside the EEPORM*/

				if (g_passCheck != 1) {
					/*If the two passwords do not match then repeat step 1 again*/
					createPass();
					if (g_passCheck != 1) {
						/* If the two passwords do not match for the second time
						 then show an ERROR and close the system*/
						LCD_clearScreen();
						LCD_displayStringRowColumn(0, 0, "Password does");
						LCD_displayStringRowColumn(1, 0, "not match");
						return 0;
					}
				}
			}

			break;
		}

	}
}
