#include <stdint.h>
#include "../inc/BSP.h"
#include "../inc/CortexM.h"
#include "../inc/msp432p401r.h"

//void BSP_buzzer_init(uint16_t duty){
//	static uint32_t SubsystemFrequency = 3000000;
//	static uint16_t PWMCycles;
//	//uint16_t tone_frequency = pwm;
//  if(duty > 1023){
//    return;                        // invalid input
//  }
//  TA0CTL &= ~0x0030;               // halt Timer A0

//  TA0CTL = 0x0200;

//  TA0CCTL4 = 0x00E0;
//  PWMCycles = SubsystemFrequency / 2048;
//  TA0CCR4 = (duty*PWMCycles)>>10;  // defines when output signal is cleared
//  TA0CCR0 = PWMCycles - 1;         // defines when output signals are set
//  TA0EX0 &= ~0x0007;               // configure for input clock divider /1
//  TA0CTL |= 0x0014;                // reset and start Timer A0 in up mode
//  // ***************** GPIO initialization *****************
//  P2SEL0 |= 0x80;
//  P2SEL1 &= ~0x80;                 // configure P2.7 as timer out
//  P2DIR |= 0x80;                   // make P2.7 out
//}


int main (){
//	//Part 1
//	uint32_t pwm = 2048;
//	uint16_t x_pos, y_pos;
//	uint8_t select;
	
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;										//Disbale Watchdog Timer
//	BSP_Joystick_Init();
//	BSP_Joystick_Input(&x_pos, &y_pos, &select);

//	
//	//Change the PWM values
//	
//	// Part 2
//	
//	
//	
//	
//	BSP_buzzer_init(512);
	BSP_Buzzer_Init(50);
	BSP_Buzzer_Set(0);
	return 0;
	
}

