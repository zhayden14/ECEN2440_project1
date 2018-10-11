#include "msp.h"


int background;
//color of the floor, as measured by sensors 0 and 7

int irct[8];


void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer

	//setup port 7
	P7SEL0 = 0x00;
	P7SEL1 = 0x00;
	P7DS = 0xFF;
	P7OUT = 0xFF;

	//configure Timer A 0
	TIMER_A0->CCTL[0] = 0x0080;    // CCI0 toggle
	TIMER_A0->CCR[0] =  20;    // Period or about 10 kHz
	TIMER_A0->EX0 =     0x0005;    // Divide by 6
	TIMER_A0->CCTL[1] = 0x0040;    // CCR1 toggle/reset
	TIMER_A0->CCTL[2] = 0x0040;
	TIMER_A0->CCR[1] =  10;    // CCR1 duty cycle is 50%
	TIMER_A0->CCR[2] =  10;
	TIMER_A0->CTL =     0x0250;    // SMCLK=12MHz, divide by 2, up mode

	/*Use of port 2:
	 * 0     left motor sleep
	 * 1     right motor sleep
	 * 2     left motor direction
	 * 3     right motor direction
	 * 4     left motor PWM (TA0 CCR1)
	 * 5     right motor PWM (TA0 CCR2)
	 * 6     unused
	 * 7     unused
	 */

	//configure GPIO pins
	P2DIR = 0x3F;    //first 6 pins in port are outputs
	P2SEL0 = 0x30;   //pins 4 and 5 are PWM from TA0
	P2SEL1 &= ~0x3F;
	P2DS   = 0x3F;
	P2OUT = 0x03;

	int i;

	while(1){
	    //take reflectance reading
	    irct[0] = 0;
	    irct[1] = 0;
	    irct[2] = 0;
	    irct[3] = 0;
	    irct[4] = 0;
	    irct[5] = 0;
	    irct[6] = 0;
	    irct[7] = 0;

	    P7DIR = 0xFF;
	    for(i = 0; i < 400; i++){}
	    P7DIR = 0x00;
	    while(P7IN){
	        if(P7IN&BIT0) irct[0]++;
	        if(P7IN&BIT1) irct[1]++;
	        if(P7IN&BIT2) irct[2]++;
	        if(P7IN&BIT3) irct[3]++;
	        if(P7IN&BIT4) irct[4]++;
	        if(P7IN&BIT5) irct[5]++;
	        if(P7IN&BIT6) irct[6]++;
	        if(P7IN&BIT7) irct[7]++;
	    }

	    //set motor speeds


	    for(i = 0; i < 4000; i++){}
	}
}
