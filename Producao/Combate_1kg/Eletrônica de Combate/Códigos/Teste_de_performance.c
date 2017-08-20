#include <msp430.h> 

/*
     This is a code designed to help engineers to
 calculate the time execution of a function,  ba-
 sed on the Capture event of timerA.

 Author: Daniel Carvalho de Sousa
 Date: 20/08/2017

 */
long int x0,y0;
int y = 100;
int x = 500;
unsigned int T0 = 0,enable = 0;
float Temp_exec = 0;
long map(long Accel, long in_min, long in_max, long out_min, long out_max)
{
  return (Accel - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
int main(void) {
    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer
    BCSCTL1 = CALBC1_1MHZ;  // select 1Mhz clock.
    DCOCTL = CALDCO_1MHZ;   // select the DCO clock.
	TA0CCTL0 = CAP + SCS + CCIS1 + CM_3 + CCIE; // both edges capture + sync with the clock + interrupt flag.
    __bis_SR_register(GIE); // set the global interruptions
	TA0CTL = TASSEL_2 + MC_2; // timer´s clock is system master clock, whithout division.
	TA0CCTL0 ^= CCIS0;  // Interrupt request (capture condition)
	TA0CTL = TASSEL_2 + MC_2;   // Reconfigure timer

	//---- Insert the code test here ! -----
	x0 = map(x,0,1000,0,100);
	y0 = map(y,0,100,0,1000);
	//--------------------------------------

	TA0CCTL0 ^= CCIS0;  // Last interrupt request (final capture condition)
	Temp_exec *= 0.001;
	//__bis_SR_register(LPM4_bits);
	return 0;
}

#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A(void){
    TA0CTL &= ~MC_3;    // Stop the timer (holds its value until the start)
    if(enable == 0){
        enable ++;
        T0 = TA0CCR0 + 9; //reason: calling an interrupt and "stop timer command" demand 9 clock cycles.
    }else{
        enable = 0;
        Temp_exec = TA0CCR0 - T0 - 4; // 4 clocks demanded of the interrupt function.
    }
    TA0CCTL0 &= ~CCIFG;
}
