#include <stdint.h>
#include "msp.h"

//Global Variable Definitions, because scope is only this file
																				//Counter variable
#define DELAY  75000																	//Delay for Bouncing, selectted after trying out a lots of values
uint8_t RED_LED   = 1;																		//Variable to keep track of the status of RED_LED
uint8_t RGB_LED   = 0;																		//Variable to keep track of the status of RGD_LED


#define OFF        (uint8_t) ((0<<2) | (0<<1) | (0<<0))							//0000 0000
#define RED        (uint8_t) ((0<<2) | (0<<1) | (1<<0))							//0000 0001
#define GREEN      (uint8_t) ((0<<2) | (1<<1) | (0<<0))							//0000 0010
#define YELLOW     (uint8_t) ((0<<2) | (1<<1) | (1<<0))							//0000 0011
#define BLUE       (uint8_t) ((1<<2) | (0<<1) | (0<<0))							//0000 0100
#define PINK       (uint8_t) ((1<<2) | (0<<1) | (1<<0))							//0000 0101
#define SKYBLUE    (uint8_t) ((1<<2) | (1<<1) | (0<<0))							//0000 0110
#define WHITE      (uint8_t) ((1<<2) | (1<<1) | (1<<0))							//0000 0111

uint8_t colors[8] = {OFF, RED, GREEN, YELLOW, BLUE, PINK, SKYBLUE, WHITE};					//Array for colours

void PORT1_IRQHandler(void){
	static uint32_t i;
	static uint8_t j = 0;
	for ( i = 0; i < DELAY; i++){__asm volatile ("");}
	if (!(P1IN & (uint8_t) (1<<1))){													//Here, Button 1 Pressed
			
		P1IFG &= (uint8_t) ~ BIT1;
			if (RGB_LED == 1 && RED_LED == 0){												//Handle Button 1, Change the control between two LEDs
				RGB_LED = 0;																//Set RED_LED = 1
				RED_LED = 1;			
			}
			else if (RGB_LED == 0 && RED_LED == 1) {										//Set RGB_LED = 1
				RGB_LED = 1;
				RED_LED = 0;
			}
																				//Ignore other conditions, very less likely for the code to reach here, but good to have
		}
		
		else if (!(P1IN & (uint8_t) (1<<4))){												//Here, Button 2 is Pressed
		P1IFG &= (uint8_t) ~ BIT4;
			if (RGB_LED == 0 && RED_LED == 1) {												//If RED_LED was selected by Button 1, THEN
				P1OUT ^= (uint8_t) (BIT0);													//Toggle the LED on bit 0 of Port 1
			}
			else if (RGB_LED == 1 && RED_LED == 0){											//IF RGB_LED was selected byt Button 1, THEN
				P2OUT &=(uint8_t) ~(((1<<2) | (1<<1) | (1<<0)));							//Clear the previous state of P2OUT by clearing the last 3 bits
				j = j % 8;
				P2OUT |= (uint8_t) (colors[j]);												//Make sure the 8 bit hex is properly casted to 8 bit int

				j++;																	//For any other state, Cycle through 8 states of RGB colours
			}
																				//Ignore unexpected conditions and continue,				 
		}
		
	
}	



int main (){
	
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;
	//Port Configurations
	//Port1
	P1SEL0 &= (uint8_t)(~((1<<4) | (1<<1) | (1<<0)));										//Clearing bits 0,1,4, to set them as GPIO
	P1SEL1 &= (uint8_t)(~((1<<4) | (1<<1) | (1<<0)));										//Clearing bits 0,1,4
	P1DIR  &= (uint8_t)(~((1<<4) | (1<<1)));												//P1.1 and P1.4 are inputs, Set the DIR register for INTPUT by clearing bits 1,4 
	P1DIR  |= (uint8_t)(1<<0);																//P1.0 is output. Set the DIR register for OUTPUT by setting bit 1 
	P1OUT  &=~(uint8_t)(1<<0);																//P1.0 is LED and Activee high, so clear bit 0
	P1OUT  |= (uint8_t)((1<<4) | (1<<1));													//P1.1 and P1.4 are switches,  Active low, so set to 1
	P1REN  |= (uint8_t)((1<<4) | (1<<1));													//P1.1 and P1.4 swtiches need internal resistor, pull-up, set the bits to 1
	
	//Port2 
	P2SEL0 &= (uint8_t)(~((1<<2) | (1<<1) | (1<<0)));										//Clearing bits 0,1,2, to set them as GPIO
	P2SEL1 &= (uint8_t)(~((1<<2) | (1<<1) | (1<<0)));										//Clearing bits 0,1,2
	P2DIR  |= (uint8_t)(((1<<2) | (1<<1) | (1<<0)));										//P2.0, 2.1, 2.2 are ouputs, Set the DIR register for OUTPUT by setting the bits to1
	P2OUT  &=~(uint8_t)(((1<<2) | (1<<1) | (1<<0)));										//P2.0, 2.1, 2.2 aer LED and Active high, so clear the bits 0

	
	//Configure Device Interrupts
	P1IES |= (uint8_t) ((1<<4) | (1<<1));
	P1IFG &= (uint8_t) ~((1<<4) | (1<<1));
	P1IE |= (uint8_t) ~((1<<4) | (1<<1));
	
	//NVIC
	NVIC_SetPriority(PORT1_IRQn, 2);
	NVIC_ClearPendingIRQ(PORT1_IRQn);
	NVIC_EnableIRQ(PORT1_IRQn);
	//Global
	__ASM("CPSIE I");
	
	
	while (1){
	__ASM("WFI");
	}
	
	

}

