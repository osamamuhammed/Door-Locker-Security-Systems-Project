/******************************************************************************
 *
 * Module: PWM
 *
 * File Name: PWM.c
 *
 * Description: Source file for the AVR PWM Driver
 *
 * Author: Osama Muhammed
 *
*******************************************************************************/
#include "avr/io.h" /* To use the IO Ports Registers */
#include "common_macros.h" /* To use the macros like SET_BIT */
#include "gpio.h"

/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/

/*
 * Description :
 * Initialize the PWM
 */
void PWM_Timer0_Start(uint8 duty_cycle){

	TCNT0 = 0; //Set Timer Initial value

	// Set Compare Value
		switch (duty_cycle)
		{
		case 0 :
			OCR0  = 0;
			break;

		case 25 :
			OCR0  = 64;
			break;

		case 50 :
			OCR0  = 128;
			break;

		case 75 :
			OCR0  = 192;
			break;

		case 100 :
			OCR0  = 255;
			break;

		default :
			break;

		}
		 // Set Compare Value

		GPIO_setupPinDirection(PORTB_ID, PIN3_ID, PIN_OUTPUT); //set PB3/OC0 as output pin --> pin where the PWM signal is generated from MC.


		/* Configure timer control register
		 * 1. Fast PWM mode FOC0=0
		 * 2. Fast PWM Mode WGM01=1 & WGM00=1
		 * 3. Clear OC0 when match occurs (non inverted mode) COM00=0 & COM01=1
		 * 4. clock = F_CPU/8 CS00=0 CS01=1 CS02=0
		 */
		TCCR0 = (1<<WGM00) | (1<<WGM01) | (1<<COM01) | (1<<CS01);

}

