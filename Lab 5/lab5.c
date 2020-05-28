#include <stdint.h>
#include "../inc/BSP.h"
#include "../inc/CortexM.h"
#include "../inc/msp432p401r.h"

static uint32_t SubsystemFrequency = 3000000; // cycles/second
static uint32_t PWMCycles;  
static uint32_t tone_frequency = 0;



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

uint32_t select_frequency (uint16_t x_pos, uint16_t y_pos){

	if ((x_pos >= 1 && x_pos < 341) && (y_pos >1 && y_pos < 341)) {
		return  1480;				//Q6
	}
	else if ((x_pos >=341 && x_pos < 682) && (y_pos >=1 && y_pos < 341)) {
		return 1661;				//Q7
	}
	else if ((x_pos >=682 && x_pos < 1023) && (y_pos >=1 && y_pos < 341)) {
		return 1760;				//Q8
	}
	else if ((x_pos >=1 && x_pos < 341) && (y_pos >=341 && y_pos < 682)) {
		return 1175;				//Q4
	}
	
	else if ((x_pos >=1 && x_pos < 341) && (y_pos >=682 && y_pos < 1023)) {
		return 880;				//Q1
	}
		
	else if ((x_pos >=341 && x_pos < 682) && (y_pos >=682 && y_pos < 1023)) {
		return  988;				//Q2
	}
	else if ((x_pos >=682 && x_pos < 1023) && (y_pos >=682 && y_pos < 1023)) {
		return 988;				//Q3
	}
			else if ((x_pos >=682 && x_pos < 1023) && (y_pos >=341 && y_pos < 682)) {
		return  988;				//Q5
			}
			else if ((x_pos >=341 && x_pos < 682) && (y_pos >=341 && y_pos < 682)) {
		return 988;				//Q0
			}
			else return 2048;
}

int main (){
//	//Part 1
	uint32_t pwm = 0;
	uint16_t x_pos = 0;
	uint16_t y_pos = 0;
	uint8_t select;			//not selectted
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;										//Disbale Watchdog Timer
	
	
	BSP_Joystick_Init();
	while (1){
			//Wait for the Joystick to be pressed
			BSP_Joystick_Input(&x_pos, &y_pos, &select);
		
		pwm = select_frequency(x_pos, y_pos);
		if (select == 0){
				BSP_buzzer_init(50, pwm);
				BSP_buzzer_set(0);
				//select = 1; 								//un select the joy stick
		}
		
		
	}



	return 0;
	
}

