/*
SYSC 3310 Lab 4
Tanvir hossain, 101058988

Please NOTE: the comments may look different on different editors,
specially in Keil Editor. I have used Visual Studio Code to make
it look neat so that the code looks same on all modern editors.
*/

#include <stdint.h>
#include "msp.h"
																				
#define DELAY      75000													    	//Delay for Bouncing, selectted after trying out a lots of values
#define OFF        (uint8_t) ((0<<2) | (0<<1) | (0<<0))								//0000 0000
#define RED        (uint8_t) ((0<<2) | (0<<1) | (1<<0))								//0000 0001
#define GREEN      (uint8_t) ((0<<2) | (1<<1) | (0<<0))								//0000 0010
#define YELLOW     (uint8_t) ((0<<2) | (1<<1) | (1<<0))								//0000 0011
#define BLUE       (uint8_t) ((1<<2) | (0<<1) | (0<<0))								//0000 0100
#define PINK       (uint8_t) ((1<<2) | (0<<1) | (1<<0))								//0000 0101
#define SKYBLUE    (uint8_t) ((1<<2) | (1<<1) | (0<<0))								//0000 0110
#define WHITE      (uint8_t) ((1<<2) | (1<<1) | (1<<0))								//0000 0111

uint8_t colors[8] = {OFF, RED, GREEN, YELLOW, BLUE, PINK, SKYBLUE, WHITE};			//Array for colours

/*
 This function handles the ISR for PORT1
*/

void PORT1_IRQHandler(void){														
	// Variable Declarations														//Using static variables 
	static uint32_t i;																//Variable to use in the delay for bouncing
	static uint8_t j = 0;															//Variable to keep track of the colours
	static uint8_t RED_LED   = 1;													//Variable to keep track of the status of RED_LED
	static uint8_t RGB_LED   = 0;													//Variable to keep track of the status of RGD_LED

	for ( i = 0; i < DELAY; i++){__asm volatile ("");}								//Creating a forced delay to deal with the switch 
																					//nouncing. Also, letting the compiler to know that
																					//this loop is important

	if (!(P1IN & (uint8_t) (1<<1))){												//Code reaches here if Button 1 Pressed
		P1IFG &= (uint8_t) ~ BIT1;													//Clearing the Flag for Port 1 bit 1

			if (RGB_LED == 1 && RED_LED == 0){										//Handle Button 1, Change the control between two LEDs
				RGB_LED = 0;														//Set RED_LED = 1
				RED_LED = 1;			
			}
			else if (RGB_LED == 0 && RED_LED == 1) {								//Set RGB_LED = 1
				RGB_LED = 1;
				RED_LED = 0;
			}
		}
		
		else if (!(P1IN & (uint8_t) (1<<4))){										//if code reaches here it means Button 2 is Pressed
		P1IFG &= (uint8_t) ~ BIT4;													//Clearing the flag of PORT 1 bit 4

			if (RGB_LED == 0 && RED_LED == 1) {										//If RED_LED was selected by Button 1, THEN
				P1OUT ^= (uint8_t) (BIT0);											//Toggle the LED on bit 0 of Port 1
			}
			else if (RGB_LED == 1 && RED_LED == 0){									//IF RGB_LED was selected byt Button 1, THEN
				P2OUT &=(uint8_t) ~(((1<<2) | (1<<1) | (1<<0)));					//Clear the previous state of P2OUT by clearing the last 3 bits
				j = j % 8;															//Modulo calculation to loop over the array for colours
				P2OUT |= (uint8_t) (colors[j]);										//Make sure the 8 bit hex is properly casted to 8 bit int
				j++;																//Increase the value of j to fetch next color from the array
			}				 
		}
		
	
}	

/*
This function configures all the pins for PORT1
*/
void configure_PORT1 (void){
	P1SEL0 &= (uint8_t) (~((1<<4) | (1<<1) | (1<<0)));								//Clearing bits 0,1,4, to set them as GPIO
	P1SEL1 &= (uint8_t) (~((1<<4) | (1<<1) | (1<<0)));								//Clearing bits 0,1,4
	P1DIR  &= (uint8_t) (~((1<<4) | (1<<1)));										//P1.1 and P1.4 are inputs, Set the DIR register for INTPUT by clearing bits 1,4 
	P1DIR  |= (uint8_t) (1<<0);														//P1.0 is output. Set the DIR register for OUTPUT by setting bit 1 
	P1OUT  &= (uint8_t) (~(1<<0));													//P1.0 is LED and Activee high, so clear bit 0
	P1OUT  |= (uint8_t) ((1<<4) | (1<<1));											//P1.1 and P1.4 are switches,  Active low, so set to 1
	P1REN  |= (uint8_t) ((1<<4) | (1<<1));											//P1.1 and P1.4 swtiches need internal resistor, pull-up, set the bits to 1
}

/*
This function configures all the pins for PORT2
*/
void configure_PORT2(void){
	P2SEL0 &= (uint8_t) (~((1<<2) | (1<<1) | (1<<0)));								//Clearing bits 0,1,2, to set them as GPIO
	P2SEL1 &= (uint8_t) (~((1<<2) | (1<<1) | (1<<0)));								//Clearing bits 0,1,2
	P2DIR  |= (uint8_t) ( (1<<2)  | (1<<1) | (1<<0) );								//P2.0, 2.1, 2.2 are ouputs, Set the DIR register for OUTPUT by setting the bits to1
	P2OUT  &= (uint8_t) (~((1<<2) | (1<<1) | (1<<0)));								//P2.0, 2.1, 2.2 aer LED and Active high, so clear the bits 0
}

/*
This function configures pin 1 and 4 of PORT1 to enbale Interrupts 
*/
void configure_Interrupts(){
	P1IES |= (uint8_t) ((1<<4) | (1<<1));											//Edge Select, for switches we set the bits
	P1IFG &= (uint8_t) ~((1<<4) | (1<<1));											//Clear the previous flags
	P1IE  |= (uint8_t) ~((1<<4) | (1<<1));											//Enable Interrupts by setting pin 1 and 4 to 1
	
																					//Configure NVIC
	NVIC_SetPriority(PORT1_IRQn, 2);												//Encode the prioirity as 2
	NVIC_ClearPendingIRQ(PORT1_IRQn);												//Clear previous IRQ
	NVIC_EnableIRQ(PORT1_IRQn);														//Enable Interrupt
																	//Enable Interrupt Globally
	
}




void TA0_N_IRQHandler(void){			
	TA0CTL &= (uint16_t) ~ (BIT0);	//Clear the TAIFG flag
	P1OUT ^= (uint8_t) BIT0;
	
}

/*
Main Function
*/
int main (){
	
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;													//Disbale Watchdog Timer

	configure_PORT1();																									//Call the function to configure Port1
	//configure_PORT2();																								//Call the function to configure Port2
	//configure_Interrupts();																						//Configure Interrupts
	
	TA0CTL &=~(uint16_t)(BIT0); 																				//BIT 0 = 0 for TAIFG flag
	TA0CTL |= (uint16_t)(BIT1); 																				//BIT 1 For enabling interrupt
	TA0CTL &= (uint16_t)(~((1<<5) | (1<<4)));														//BIT 5, 4 = 0 0	to Set the mode
	TA0CTL |= (uint16_t) (BIT4);																				//At this point, BIT 5,4 = 0 1 , Up mode 
	
	TA0CTL &=~ (uint16_t) ((BIT7) | (BIT6));														//BIT 7,6 = 0 0, Divide by 1, for ID
	
	TA0CTL &=~ (uint16_t) (BIT9);																				//BIT 9,8 = 0, 0 , MCLK  for TASSSEL
	TA0CTL &=~ (uint16_t) (BIT8);	

	TA0EX0 &=~ (uint16_t) ((BIT2) | (BIT1)| (BIT0));  									// 000 divide by 1
	TA0CCR0 = (uint16_t) (2999999);																				
	TA0CTL |= (uint16_t)(BIT2);
	
	__ASM("CPSIE I");
	while (1){																													//This while loop keeps the main function running forever
	__ASM("WFI");																												//This in-line assembly ensures that the loop is waiting 
	}																																		//for an interrupt. It also ensures that while waiting the 
																																			//processor is reducing power usage.
	return 0;
}

