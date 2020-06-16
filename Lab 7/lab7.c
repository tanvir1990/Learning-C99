/*
sysc 3310, Lab7
Tanvir Hossain, 101058988
*/
																						//Includes are set up according to the lab instructions
#include <stdint.h>
#include "../inc/BSP.h"
#include "../inc/CortexM.h"
#include "../inc/msp432p401r.h"
#include "fate.h"
#include "msp.h"

#define RED        (uint8_t) ((0<<2) | (0<<1) | (1<<0))
#define ST7735_RED     0xF800
#define ST7735_YELLOW  0xFFE0
#define ST7735_BLACK   0x0000

void configure_TA0CTL_bits(){
																											//For TA0 we will be using TAIFG flag
	TA0CTL &=~ (uint16_t) (BIT0); 											//BIT 0 = 0 for clearing previous TAIFG flag, 
	//TA0CTL |=  (uint16_t) (BIT1); 										//BIT 1 For enabling interrupt
	TA0CTL |=  (uint16_t) (BIT2);												//BIT 2, Reset the counter
	TA0CTL &=~ (uint16_t) ((BIT7) | (BIT6));						//For ID BIT 7,6 = 0 0, Divide by 1, 
	TA0EX0 &=~ (uint16_t) ((BIT2) | (BIT1)| (BIT0));  			// 000 divide by 1
	
	TA0CTL &=~ (uint16_t) ((BIT9) | (BIT8));								//BIT 9,8 = 0, 1 , ACLK  for TASSSEL
	//TA0CTL |=  (uint16_t) (BIT8);	

	//TA0CCR0 =  (uint16_t) (327 - 1);							//32767 Ticks will give us 1 sec
	TA0CTL &=  (uint16_t)(~((1<<5) | (1<<4)));					//BIT 5, 4 = 0 1	to Set the mode
	//TA0CTL |=  (uint16_t) (BIT4);								//At this point, BIT 5,4 = 0 1 , Up mode 
}

void configure_PORT1 (void){
	P1SEL0 &=  (uint8_t) (~(1<<0));								//Clearing bits 0 to set LED P1.0 as GPIO
	P1SEL1 &=  (uint8_t) (~ (1<<0));								//Clearing bits 0
	P1DIR  |=  (uint8_t) (1<<0);									//P1.0 is output. Set the DIR register for OUTPUT by setting bit 1 
	P1OUT  &=~ (uint8_t) ((1<<0));								//P1.0 is LED and Activee high, so clear bit 0
}

void configure_Port1_Interrupts(){
	P1IES &=~ (uint8_t) ((1<<4) | (1<<1));											//Edge Select, for switches we set the bits, changed to rising edge
	P1IFG &= (uint8_t) ~((1<<4) | (1<<1));											//Clear the previous flags
	P1IE  |= (uint8_t) ~((1<<4) | (1<<1));											//Enable Interrupts by setting pin 1 and 4 to 1
	
																															//Configure NVIC
	NVIC_SetPriority(35, 2);																		//Encode the prioirity as 2
	NVIC_ClearPendingIRQ(PORT1_IRQn);														//Clear previous IRQ
	NVIC_EnableIRQ(PORT1_IRQn);																	//Enable Interrupt
}

void printNumber(){
	uint32_t newX = 0;
	uint32_t newY = 0;
	uint16_t textColor = ST7735_YELLOW;
	
	uint16_t width = 128;
	uint16_t height = 128;
	uint16_t color = ST7735_BLACK;
	static uint32_t n;
	//n = n + (uint32_t)1;
	n =0;
	BSP_LCD_FillRect(0, 0, width, height, color);
	
	BSP_LCD_SetCursor(newX, newY);
	BSP_LCD_OutUDec(n,  textColor);
	
	P1OUT ^= (uint8_t)BIT0;
	Task_stop((uint32_t)printNumber);
	
}

int main (){
	
	uint32_t period = 200;
	uint32_t priority = 1;


	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;
	
	BSP_LCD_Init();
	
	Task_list_init();
	Task_add( (uint32_t) printNumber, period, priority);
	
	configure_PORT1();
	configure_TA0CTL_bits();
	Task_schedule();	
	
		

	return 0;

}
