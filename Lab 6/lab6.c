/*
SYSC 3310
Tanvir Hossain, 101058988
Date: 4th June, 2020
*/

#include <stdint.h>
#include "msp.h"

#define OFF        (uint8_t) ((0<<2) | (0<<1) | (0<<0))			//0000 0000
#define RED        (uint8_t) ((0<<2) | (0<<1) | (1<<0))			//0000 0001
#define GREEN      (uint8_t) ((0<<2) | (1<<1) | (0<<0))			//0000 0010
#define YELLOW     (uint8_t) ((0<<2) | (1<<1) | (1<<0))			//0000 0011
#define BLUE       (uint8_t) ((1<<2) | (0<<1) | (0<<0))			//0000 0100
#define PINK       (uint8_t) ((1<<2) | (0<<1) | (1<<0))			//0000 0101
#define SKYBLUE    (uint8_t) ((1<<2) | (1<<1) | (0<<0))			//0000 0110
#define WHITE      (uint8_t) ((1<<2) | (1<<1) | (1<<0))			//0000 0111

uint8_t colors[8] = {OFF, RED, GREEN, YELLOW, BLUE, PINK, SKYBLUE, WHITE};			//Array for colours

/*
	This function configures all the pins for PORT1
*/
void configure_PORT1 (void){
	P1SEL0 &=  (uint8_t) (~(1<<0));								//Clearing bits 0 to set LED P1.0 as GPIO
	P1SEL1 &=  (uint8_t) (~ (1<<0));								//Clearing bits 0
	P1DIR  |=  (uint8_t) (1<<0);									//P1.0 is output. Set the DIR register for OUTPUT by setting bit 1 
	P1OUT  &=~ (uint8_t) ((1<<0));								//P1.0 is LED and Activee high, so clear bit 0
}
/*
	This function configures all the pins for PORT2
*/
void configure_PORT2(void){
	P2SEL0 &= (uint8_t) (~((1<<2) | (1<<1) | (1<<0)));			//Clearing bits 0,1,2, to set them as GPIO
	P2SEL1 &= (uint8_t) (~((1<<2) | (1<<1) | (1<<0)));			//Clearing bits 0,1,2
	P2DIR  |= (uint8_t) ( (1<<2)  | (1<<1) | (1<<0) );			//P2.0, 2.1, 2.2 are ouputs, Set the DIR register for OUTPUT by setting the bits to1
	P2OUT  &= (uint8_t) (~((1<<2) | (1<<1) | (1<<0)));			//P2.0, 2.1, 2.2 aer LED and Active high, so clear the bits 0
}
/*
	This function Initiliazes NVIC for TA0 and TA1 
*/
void configure_TimerA_Interrupts(){
	NVIC_SetPriority(TA0_N_IRQn, 2);							//Configure NVIC for TA0
	NVIC_ClearPendingIRQ(TA0_N_IRQn);
	NVIC_EnableIRQ(TA0_N_IRQn);
	
	NVIC_SetPriority(TA1_0_IRQn, 2);							//Configure NVIC for TA1
	NVIC_ClearPendingIRQ(TA1_0_IRQn);
	NVIC_EnableIRQ(TA1_0_IRQn);
}

/*
	This function configures all the necessary bits for
	TA0
*/

void configure_TA0CTL_bits(){
																//For TA0 we will be using TAIFG flag
	TA0CTL &=~ (uint16_t) (BIT0); 								//BIT 0 = 0 for clearing previous TAIFG flag, 
	TA0CTL |=  (uint16_t) (BIT1); 								//BIT 1 For enabling interrupt
	TA0CTL |=  (uint16_t) (BIT2);									//BIT 2, Reset the counter
	TA0CTL &=~ (uint16_t) ((BIT7) | (BIT6));					//For ID BIT 7,6 = 0 0, Divide by 1, 
	TA0EX0 &=~ (uint16_t) ((BIT2) | (BIT1)| (BIT0));  			// 000 divide by 1
	
	TA0CTL &=~ (uint16_t) (BIT9);								//BIT 9,8 = 0, 1 , ACLK  for TASSSEL
	TA0CTL |=  (uint16_t) (BIT8);	

	TA0CCR0 =  (uint16_t) (32768 - 1);							//32767 Ticks will give us 1 sec
	TA0CTL &=  (uint16_t)(~((1<<5) | (1<<4)));					//BIT 5, 4 = 0 1	to Set the mode
	TA0CTL |=  (uint16_t) (BIT4);								//At this point, BIT 5,4 = 0 1 , Up mode 
}

/*
	This function configures all the necessary bits for
	TA1
*/

void configure_TA1CTL_bits(){
																//For TA1 we will be using CCIFG Flag
	TA1CCTL0 |= (uint16_t) BIT4;								//Enable The Interrupt, BIT 4
	TA1CCTL0 &=~ (uint16_t) BIT0;								//Clear the previous flag
	TA1CTL |= (uint16_t) (BIT2);								//BIT 2, Reset the counter
	TA1CTL &=~ (uint16_t) ((BIT7) | (BIT6));					//For ID BIT 7,6 = 0 0, Divide by 1, 
	TA1EX0 &=~ (uint16_t) ((BIT2) | (BIT1)| (BIT0));  			// 000 divide by 1
	
	TA1CTL &=~ (uint16_t) (BIT9);								//BIT 9,8 = 0, 1 , ACLK  for TASSSEL
	TA1CTL |= (uint16_t) (BIT8);	


	TA1CCR0 = (uint16_t) (3268 - 1);							//Generate interrupt after 0.1 sec
	
	TA1CTL &= (uint16_t)(~((1<<5) | (1<<4)));					//BIT 5, 4 = 0 0	to Set the mode
	TA1CTL |= (uint16_t) (BIT5) | (BIT4);						//At this point, BIT 5,4 = 1 1 , Up/Down mode  
}


/*
	ISR for Timer A0 Interrupts
*/	
void TA0_N_IRQHandler(void){									//This handler will toggle P1.0 LED every second
	TA0CTL &=~(uint16_t)(BIT0); 								//Clears the previous flag
	P1OUT ^= (uint8_t) BIT0;
}
/*
	ISR for Timer A1 Interrupts
*/	
void TA1_0_IRQHandler(void){
	static uint8_t j = 0;										//This handler will toggle RGB LED among 7 different colors every 0.5 sec
	TA1CTL |= (uint16_t)(BIT2);									// Reset the counter, since we have an initial delay
	TA1CCTL0 &=~ (uint16_t) BIT0;								//Clear the previous flag
	
	P2OUT &=(uint8_t) ~(((1<<2) | (1<<1) | (1<<0)));			//Clear the previous state of RGB LEDs
	j = j % 8;													//Modulo calculation to loop over the array for colours
	P2OUT |= (uint8_t) (colors[j]);								//Make sure the 8 bit hex is properly casted to 8 bit int
	j++;
	TA1CCR0 = (uint16_t) (16384 -1 );	 						//After first interrupt, set TA1CCR to count up to 0.5 sec
																//16384 ticks gives us 0.1 sec
	
}

/*
	Main function
*/
int main (){
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;					//Disbale Watchdog Timer

	configure_PORT1();											//Call the functions to configure Ports
	configure_PORT2();	
	P1OUT  |= (uint8_t) ((1<<0));								//At time 0, we want RED LED to be ON
	configure_TA0CTL_bits();
	configure_TA1CTL_bits();
	configure_TimerA_Interrupts();

	__ASM("CPSIE I");
	while (1){																							
		__ASM("WFI");											//Let the compiler know that this Loop is importatnt		
	}																												
																													
	return 0;
}
