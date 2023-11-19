/******************************************************************************
 *
 * Module: Timer/Counter1
 *
 * File Name: timer1.c
 *
 * Description: Source file for the AVR Timer/Counter1
 *
 * Author: Osama M. Shehata
 *
 *******************************************************************************/
#include "gpio.h"
#include "common_macros.h"
#include "Buzzer.h"


/*******************************************************************************
 *                      Functions Definitions                                  *
*******************************************************************************/

/*
 * Description:
 * 		Setup the direction for the buzzer pin as output pin through the GPIO driver.
 * 		Turn off the buzzer through the GPIO.
 * Inputs: None
 * Return: None
 */
void Buzzer_init()
{
	/* Setup the direction for the buzzer pin as output pin through the GPIO driver */
	GPIO_setupPinDirection(PORTD_ID, PIN7_ID, PIN_OUTPUT);

	/* Turn off the buzzer through the GPIO */
	GPIO_writePin(PORTD_ID, PIN7_ID, LOGIC_LOW);
}

/*
 * Description: Function to enable the Buzzer through the GPIO.
 * Inputs: None
 * Return: None
*/
void Buzzer_on(void)
{
	/* Turn ON the buzzer through the GPIO */
	GPIO_writePin(PORTD_ID, PIN7_ID, LOGIC_HIGH);
}

/*
 * Description: Function to disable the Buzzer through the GPIO.
 * Inputs: None
 * Return: None
*/
void Buzzer_off(void)
{
	/* Turn OFF the buzzer through the GPIO */
	GPIO_writePin(PORTD_ID, PIN7_ID, LOGIC_LOW);
}
