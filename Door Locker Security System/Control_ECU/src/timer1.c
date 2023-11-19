/******************************************************************************
 *
 * Module: timer/Counter1
 *
 * File Name: timer1.c
 *
 * Description: Source file for the AVR Timer/Counter1
 *
 * Author: Osama M. Shehata
 *
 *******************************************************************************/
#include "common_macros.h" /* To use the macros like SET_BIT */
#include <avr/io.h> /*To use Timer1 registers*/
#include <avr/interrupt.h>
#include"gpio.h"
#include "timer1.h"

/*******************************************************************************
 *                           Global Variables                                  *
 *******************************************************************************/

/* Global variables to hold the address of the call back function in the application */
static volatile void (*g_callBackPtr)(void) = NULL_PTR;

/*******************************************************************************
 *                       Interrupt Service Routines                            *
 *******************************************************************************/
ISR(TIMER1_COMPA_vect) {
	if (g_callBackPtr != NULL_PTR) {
		/* Call the Call Back function in the application after the edge is detected */
		(*g_callBackPtr)(); /* another method to call the function using pointer to function g_callBackPtr(); */
	}
}

/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/
/*
 * Description : Function to initialize Timer 1 driver
 * Inputs: pointer to the configuration structure with type Timer1_ConfigType
 * Return: None
 */
void Timer1_init(const Timer1_ConfigType *Config_Ptr) {

	TCNT1 = Config_Ptr->initial_value; /* Set Timer1 initial value */

	/*
	 * 1. Set waveform generation mode to CTC (Clear timer on compare match) WGM12 =1
	 * 2. Set compare output to toggle OC01 on compare watch COM1A0 =1
	 * 3. Set the pre-scalar to 1024
	 * */

	TCCR1A = (1 << FOC1A) | (1 << FOC1B) | (1 << COM1A1);

	switch (Config_Ptr->mode) {
	case NORMAL:
		TCCR1A = (TCCR1A & 0xFC) | (Config_Ptr->mode);
		TIMSK |= (1 << TOIE1);
		break;

	case COMPARE:
		TCCR1B = (TCCR1B & 0xE7) | (Config_Ptr->mode << 1);
		/* Set compare value */
		OCR1A = Config_Ptr->compare_value;
		/* Enable Compare Match A Interrupt */
		TIMSK |= (1 << OCIE1A);
		/* Configure the pre-scalar for TIMER1 */
		TCCR1B = (TCCR1B & 0xF8) | (Config_Ptr->prescaler);
		break;
	}
}

/* Description: Function to disable the Timer1.
 * Inputs: None
 * Return: None
 */
void Timer1_deInit(void) {
	/* Clear initial value and reset all bits of Timer1 registers*/
	TCNT1 = 0;
	TCCR1A = 0x00;
	TCCR1B = 0x00;

	/* Disable all Input Capture interrupts */
	TIMSK = 0x3C;

}

/* Description: Function to set the Call Back function address.
 * Inputs: pointer to Call Back function.
 * Return: None
 */
void Timer1_setCallBack(void (*a_ptr)(void)) {
	/* Save the address of the Call back function in a global variable */
	g_callBackPtr = a_ptr;
}

