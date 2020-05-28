/*
sysc 3310, Lab5
Tanvir Hossain, 101058988
*/

#include <stdint.h>
#include "../inc/BSP.h"
#include "../inc/CortexM.h"
#include "../inc/msp432p401r.h"
																						//According to the lecture 9, we would require to use some Global Variables
static uint32_t SubsystemFrequency = 3000000; 											//Clock system cycles/second
static uint32_t PWMCycles;  															//Pulse width modulation cycles
static uint32_t tone_frequency = 0;														//Frequency for different quadrant, according to lab instruction

																						//Here two DELAYs are used for optimization
#define DELAY      300000										//Using a high DELAY value helps the tone to play constantly for longer time
#define DELAY2      15000

/*
This fucntion, inspired from BSP.c, initializes all the necessary pins for
Buzzer
input - duty cycle
input - pwm - the frequency for different quadrant, according lab manual
ouput - none
*/
void BSP_buzzer_init(uint16_t duty, uint32_t pwm){
  tone_frequency = pwm;
  PWMCycles = SubsystemFrequency/tone_frequency;
  if(duty > 1023){
    return;                        														// invalid input
  }
  TA0CTL    &= ~0x0030;               													// Halt Timer A0
  TA0CTL     = 0x0200;
  TA0CCTL4   = 0x00E0;
  TA0CCR4    = (duty*PWMCycles)>>10;  													// defines when output signal is cleared
  TA0CCR0    = PWMCycles - 1;         													// defines when output signals are set
  TA0EX0    &= ~0x0007;               													// configure for input clock divider /1
  TA0CTL    |= 0x0014;                													// reset and start Timer A0 in up mode
																						//GPIO Initialization 
  P2SEL0    |= 0x80;
  P2SEL1    &= ~0x80;                 													// configure P2.7 as timer out
  P2DIR     |= 0x80;                   													// make P2.7 out
}

/*
This function, inspired from BSP.c, sets the duty cycle for Buzzer
Assuming the buzzer_init() is already called
*/
void BSP_buzzer_set(uint16_t duty){
  if(duty > 1023){
    return;                        														// invalid input
  }
  TA0CCR4 = (duty*PWMCycles)>>10;  														// defines when output signal is cleared
}

/*
This function selects frequency given in the lab manual based on 
x and y values of the Joystick
*/
uint32_t select_frequency (uint16_t x_pos, uint16_t y_pos){

	if ((x_pos >= 1 && x_pos < 341) && (y_pos >1 && y_pos < 341)) {
		return  1480;																	//Q6
	}
	else if ((x_pos >=341 && x_pos < 682) && (y_pos >=1 && y_pos < 341)) {
		return 1661;																	//Q7
	}
	else if ((x_pos >=682 && x_pos < 1023) && (y_pos >=1 && y_pos < 341)) {
		return 1760;																	//Q8
	}
	else if ((x_pos >=1 && x_pos < 341) && (y_pos >=341 && y_pos < 682)) {
		return 1175;																	//Q4
	}
	else if ((x_pos >=1 && x_pos < 341) && (y_pos >=682 && y_pos < 1023)) {
		return 880;																		//Q1
	}
	else if ((x_pos >=341 && x_pos < 682) && (y_pos >=682 && y_pos < 1023)) {
		return  988;																	//Q2
	}
	else if ((x_pos >=682 && x_pos < 1023) && (y_pos >=682 && y_pos < 1023)) {
		return 1109;																	//Q3
	}
	else if ((x_pos >=682 && x_pos < 1023) && (y_pos >=341 && y_pos < 682)) {
		return  1319;																	//Q5
	}
	else if ((x_pos >=341 && x_pos < 682) && (y_pos >=341 && y_pos < 682)) {
		return 880;																		//Q0, frequency is 880Hz according to lab instruction, and duty cycle will be passed 0.
	}
			else return 2048;															//At other scenario, which is less likely, frequency will be 2048
}
/*
Main function
*/
int main (){

	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;											//Diable Watchdog timer
	uint32_t i;																			//An integer to count the delays
	uint32_t pwm   = 0;								 
	uint16_t x_pos = 0;
	uint16_t y_pos = 0;
	uint8_t select;

	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;											//Disbale Watchdog Timer
	
	BSP_Joystick_Init();																//Initialize Joystick Configuration
																						
																						//While loop to run forever, and polling
	while (1){
			
		BSP_Joystick_Input(&x_pos, &y_pos, &select);
		pwm = select_frequency(x_pos, y_pos);											//variable pwm stores the value of quadrant frequency								
		if ((x_pos >=341 && x_pos < 682) && (y_pos >=341 && y_pos < 682)) {				//For Quadrant 0,
			BSP_buzzer_init(0, pwm);												 	//Initialize the buzzer Duty Cycle as 0, so that buzzer is silent
			BSP_buzzer_set(0);															
			for ( i = 0; i < DELAY2; i++){__asm volatile ("");}							//Delay for dealing bouncing between Joystick contacts
		}
		else 																			//For any other quadrant,
		{BSP_buzzer_init(512, pwm);														//Switch the buzzer on at 50% duty cycle, which is 512 according to BSP.c
			for ( i = 0; i < DELAY; i++){__asm volatile ("");}							//Delay for dealing bouncing between Joystick contacts		
		}		
	}
	return 0;
	
}

