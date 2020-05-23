#include <stdint.h>
#include "msp.h"
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





/*ISR for Timer Interrupts*/
void TA0_N_IRQHandler(void){			
	TA0CTL &= (uint16_t) ~ (BIT0);	//Clear the TAIFG flag
	P1OUT ^= (uint8_t) BIT0;
}
int main (){
	
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;							//Disbale Watchdog Timer

	configure_PORT1();																			//Call the function to configure Port1

	TA0CTL &=~(uint16_t)(BIT0); 														//BIT 0 = 0 for TAIFG flag
	TA0CTL |= (uint16_t)(BIT1); 														//BIT 1 For enabling interrupt
	TA0CTL |= (uint16_t)(BIT2);															//BIT 2, Reset the counter
	TA0CTL &= (uint16_t)(~((1<<5) | (1<<4)));								//BIT 5, 4 = 0 0	to Set the mode
	TA0CTL |= (uint16_t) (BIT4);														//At this point, BIT 5,4 = 0 1 , Up mode 
	
	TA0CTL &=~ (uint16_t) ((BIT7) | (BIT6));								//For ID BIT 7,6 = 0 0, Divide by 1, 
	
	TA0CTL &=~ (uint16_t) (BIT9);														//BIT 9,8 = 0, 1 , ACLK  for TASSSEL
	TA0CTL |= (uint16_t) (BIT8);	

	TA0EX0 &=~ (uint16_t) ((BIT2) | (BIT1)| (BIT0));  			// 000 divide by 1
	TA0CCR0 = (uint16_t) (32768 - 1);																			
	
	
	__ASM("CPSIE I");
	while (1){																							
	__ASM("WFI");																						
	}																												
																													
	return 0;
}

