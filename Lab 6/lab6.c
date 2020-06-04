#include <stdint.h>
#include "msp.h"
/*
This function configures all the pins for PORT1
*/
void configure_PORT1 (void){
	P1SEL0 &= (uint8_t) (~(1<<0));						//Clearing bits 0,1,4, to set them as GPIO
	P1SEL1 &= (uint8_t) (~ (1<<0));						//Clearing bits 0,1,4
	P1DIR  |= (uint8_t) (1<<0);								//P1.0 is output. Set the DIR register for OUTPUT by setting bit 1 
	P1OUT  &= (uint8_t) (~(1<<0));						//P1.0 is LED and Activee high, so clear bit 0
}

void configure_PORT2(void){
	P2SEL0 &= (uint8_t) (~((1<<2) | (1<<1) | (1<<0)));								//Clearing bits 0,1,2, to set them as GPIO
	P2SEL1 &= (uint8_t) (~((1<<2) | (1<<1) | (1<<0)));								//Clearing bits 0,1,2
	P2DIR  |= (uint8_t) ( (1<<2)  | (1<<1) | (1<<0) );								//P2.0, 2.1, 2.2 are ouputs, Set the DIR register for OUTPUT by setting the bits to1
	P2OUT  &= (uint8_t) (~((1<<2) | (1<<1) | (1<<0)));								//P2.0, 2.1, 2.2 aer LED and Active high, so clear the bits 0
}
/*
This function configures pin 1 and 4 of PORT1 to enbale Interrupts 
*/

void configure_TimerA_Interrupts(){
	NVIC_SetPriority(TA0_N_IRQn, 2);
	NVIC_ClearPendingIRQ(TA0_N_IRQn);
	NVIC_EnableIRQ(TA0_N_IRQn);
	
	NVIC_SetPriority(TA1_N_IRQn, 2);
	NVIC_ClearPendingIRQ(TA1_N_IRQn);
	NVIC_EnableIRQ(TA1_N_IRQn);
}

void configure_TA0CTL_bits(){

	TA0CTL &=~(uint16_t)(BIT0); 														//BIT 0 = 0 for TAIFG flag
	TA0CTL |= (uint16_t)(BIT1); 														//BIT 1 For enabling interrupt
	TA0CTL |= (uint16_t)(BIT2);															//BIT 2, Reset the counter
	TA0CTL &=~ (uint16_t) ((BIT7) | (BIT6));								//For ID BIT 7,6 = 0 0, Divide by 1, 
	TA0EX0 &=~ (uint16_t) ((BIT2) | (BIT1)| (BIT0));  			// 000 divide by 1
	
	TA0CTL &=~ (uint16_t) (BIT9);														//BIT 9,8 = 0, 1 , ACLK  for TASSSEL
	TA0CTL |= (uint16_t) (BIT8);	


	TA0CCR0 = (uint16_t) (32768 - 1);	
	
	TA0CTL &= (uint16_t)(~((1<<5) | (1<<4)));								//BIT 5, 4 = 0 0	to Set the mode
	TA0CTL |= (uint16_t) (BIT4);														//At this point, BIT 5,4 = 0 1 , Up mode 
}

void configure_TA1CTL_bits(){
	
	TA1CTL &=~(uint16_t)(BIT0); 														//BIT 0 = 0 for TAIFG flag
	TA1CTL |= (uint16_t)(BIT1); 														//BIT 1 For enabling interrupt
	TA1CTL |= (uint16_t)(BIT2);															//BIT 2, Reset the counter
	TA1CTL &=~ (uint16_t) ((BIT7) | (BIT6));								//For ID BIT 7,6 = 0 0, Divide by 1, 
	TA1EX0 &=~ (uint16_t) ((BIT2) | (BIT1)| (BIT0));  			// 000 divide by 1
	
	TA1CTL &=~ (uint16_t) (BIT9);														//BIT 9,8 = 0, 1 , ACLK  for TASSSEL
	TA1CTL |= (uint16_t) (BIT8);	


	TA1CCR0 = (uint16_t) (32768 - 1);	
	
	TA1CTL &= (uint16_t)(~((1<<5) | (1<<4)));								//BIT 5, 4 = 0 0	to Set the mode
	TA1CTL |= (uint16_t) (BIT4);														//At this point, BIT 5,4 = 0 1 , Up mode  
}



/*ISR for Timer Interrupts*/	
void TA0_N_IRQHandler(void){
	TA0CTL &=~(uint16_t)(BIT0); 
	P1OUT ^= (uint8_t) BIT0;
}

void TA1_N_IRQHandler(void){
	TA1CTL &=~(uint16_t)(BIT0);
	
	//TA1CCTL0 &=~ (uint16_t) BIT0; 
	P2OUT ^= (uint8_t) BIT0;
}


int main (){
	
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;							//Disbale Watchdog Timer

	configure_PORT1();																			//Call the function to configure Port1
	configure_PORT2();	
	configure_TA0CTL_bits();
	configure_TA1CTL_bits();
	
	//configure_Port1_Interrupts();
	configure_TimerA_Interrupts();
	
	
	__ASM("CPSIE I");
	while (1){																							
	__ASM("WFI");																						
	}																												
																													
	return 0;
}