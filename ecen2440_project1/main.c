#include "msp.h"


int background;
//color of the floor, as measured by sensors 0 and 7


void PORT1_IRQHandler(void){
    //save state of IFG register to determine behavior
    char iflag = P1IV;
    int i;
    //clear interrupt flag
    P1IFG = 0;
    //switch 1
    if(iflag & 0x04){
        //pulse
        P5OUT = 0x01;
        for(i = 0; i < 10; i++){}
        P5OUT = 0x00;
        //increment interrupt count
        s1ct++;
        //LED control
        if(color & 0x04)
            //if blue LED on, turn on red
            color = 0x01;
        else
            //turn on next LED
            color = color << 1;
    }
    else if(iflag & 0x0A){
        //pulse
        P5OUT = 0x04;
        for(i = 0; i < 10; i++){}
        P5OUT = 0x00;
        //interrupt count
        s2ct++;
        //LED control
        if(color & 0x01)
            //if red LED on, turn on blue
            color = 0x04;
        else
            //turn on previous LED
            color = color >> 1;
    }
    else{
        P1OUT |= 0x01;
        //turn on red LED to indicate error
    }
    //zero all but 3 bits to LEDs and send to P2OUT
    color &= 0x07;
    P2OUT = color;
    //reset IFG again (to hopefully negate switch bounce)
    P1IFG = 0;
}


void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer

	//setup port 10
	P10SEL0 = 0x00;
	P10SEL1 = 0x00;
	P10DS = 0xFF;
	P10OUT = 0xFF;

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
	    ir2ct = 0;
	    ir5ct = 0;
	    P10DIR = 0xFF;
	    for(i = 0; i < 400; i++){}
	    P10DIR = 0x00;
	    while((P10IN&BIT1) || (P10IN&BIT3)){
	        if(P10IN&BIT1) ir2ct++;
	        if(P10IN&BIT3) ir5ct++;
	    }

	    //set motor speeds
	    if(ir2ct < 200) TIMER_A0->CCR[1] = 19;
	    else TIMER_A0->CCR[1] =  10;
	    if(ir5ct < 200) TIMER_A0->CCR[2] = 19;
	    else TIMER_A0->CCR[2] =  10;

	    for(i = 0; i < 4000; i++){}
	}
}
