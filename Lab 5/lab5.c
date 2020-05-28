#include <stdint.h>
#include "../inc/BSP.h"
#include "../inc/CortexM.h"
#include "../inc/msp432p401r.h"

static uint32_t SubsystemFrequency = 3000000; // cycles/second
static uint32_t PWMCycles;  
static uint32_t tone_frequency = 2048;


void BSP_buzzer_init(uint16_t duty, uint32_t pwm){
  if(duty > 1023){
    return;                        // invalid input
  }
  TA0CTL &= ~0x0030;               // halt Timer A0

  TA0CTL = 0x0200;
	tone_frequency = pwm;
  TA0CCTL4 = 0x00E0;
  PWMCycles = SubsystemFrequency/tone_frequency;
  TA0CCR4 = (duty*PWMCycles)>>10;  // defines when output signal is cleared
  TA0CCR0 = PWMCycles - 1;         // defines when output signals are set
  TA0EX0 &= ~0x0007;               // configure for input clock divider /1
  TA0CTL |= 0x0014;                // reset and start Timer A0 in up mode
  // ***************** GPIO initialization *****************
  P2SEL0 |= 0x80;
  P2SEL1 &= ~0x80;                 // configure P2.7 as timer out
  P2DIR |= 0x80;                   // make P2.7 out
}

void BSP_buzzer_set(uint16_t duty){
  if(duty > 1023){
    return;                        // invalid input
  }
  TA0CCR4 = (duty*PWMCycles)>>10;  // defines when output signal is cleared
}

int main (){
//	//Part 1
	uint32_t pwm = 880;
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
	BSP_buzzer_init(50, pwm);
	BSP_buzzer_set(0);
	return 0;
	
}

