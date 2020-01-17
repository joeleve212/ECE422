// Program for Project 1
//
// Joe Leveille
// Preston Smith
// 1/13/2020 v1
// for ECE-422
//-------------------

#include <msp430.h> 
#include <stdlib.h>
#define STOP_WATCHDOG 0x5A80 // Stop the watchdog timer
#define ACLK 0x0100 // Timer ACLK source
#define UP 0x0010 // Timer UP mode
#define ENABLE_PINS 0xFFFE // Required to use inputs and outputs
#define SMCLK 0x0200
#define RESET_SET 0x00E0
#define STEP_SIZE 200
int cycTotal;
float actDutyPerc, guessDutyPerc;

int main(void) {
	WDTCTL = WDTPW + WDTHOLD; 				// Stop watchdog timer
	PM5CTL0 = ENABLE_PINS; 				// Enable inputs and outputs

	P1DIR = P1DIR | BIT0; 					// P1.0 (Red LED) will be an output
	P1OUT = P1OUT & ~BIT0; 					// Turn OFF P1.0 (Red LED)

//	P9DIR = P9DIR | BIT7; 					// P9.7 (green LED) will be an output
//	P9OUT = P9OUT & ~BIT7; 					// Turn OFF P1.0 (green LED)

	P3DIR |= BIT3;   //P1.5
	P3OUT &= ~BIT3;  //P1.5

	P1DIR = P1DIR & ~BIT1; 				// P1.1 (Button S1) will be an input
	P1REN = P1REN | BIT1;					// P1.1 will have a pull-up
	P1OUT = P1OUT | BIT1; 					// resistor.

	P1DIR = P1DIR & ~BIT2; 				// P1.2 (Button S1) will be an input
	P1REN = P1REN | BIT2;					// P1.2 will have a pull-up
	P1OUT = P1OUT | BIT2; 					// resistor.

	P1DIR = P1DIR & ~BIT3; 				// P1.3 (Button S1) will be an input
	P1REN = P1REN | BIT3;					// P1.3 will have a pull-up
	P1OUT = P1OUT | BIT3; 					// resistor.

	P1IE = BIT2; 							// Enable interrupt ONLY for P1.2
	P1IES = BIT2; 							// for transitions from HI-->LO
	P1IE |= BIT1; 							// Enable interrupt ONLY for P1.1
	P1IES |= BIT1; 							// for transitions from HI-->LO
	P1IFG = 0x00; 							// Ensure no ISRs are pending

	int minVal = 4630;
	int valSpread = 13890;
	int percSpread = 80;
	int minPerc = 10;

	cycTotal = (rand() % (valSpread+1))+minVal; //randomize frequency
	actDutyPerc = (float)((rand() % (percSpread +1))+minPerc)/100.0; //randomize red light duty cycle
	TA0CCR0 = cycTotal;						// Sets value of Timer_0 to random frequency
	TA0CCR1 = TA0CCR0 * actDutyPerc;				// sets CCR1 to check for randomized duty
	TA0CTL = ACLK + UP + TACLR; 			// Start TA0 from zero with ACLK in UP MODE
	TA0CCTL1 = RESET_SET;					// Setup TA0 to output in Reset/Set mode

	P1SEL0 = P1SEL0 | 0x01;					// Setup pin P1.0 to output from the TA0 clock
	P1SEL1 = P1SEL1 & ~0x01;				// Setup pin P1.0 to output from the TA0 clock

	guessDutyPerc = (float)((rand() % (percSpread +1))+minPerc)/100.0; //randomize the green light duty cycle
	TA1CCR0 = cycTotal;						// Sets value of Timer_1 to same frequency as timer 0             ///////////////TODO: look at TB0 vs TA1
	TA1CCR1 = TA1CCR0 * guessDutyPerc;				// sets CCR1 to check for randomized duty
	TA1CTL = ACLK + UP + TACLR; 			// Start TA1 from zero with ACLK in UP MODE
	TA1CCTL1 = RESET_SET;					// Setup TA1 to output in Reset/Set mode

	P3SEL0 &= ~BIT3;					// Setup pin P3.3 to output from the TA1 clock                 ////////////////////////TODO: CHECK INTO P9SEL0 VS P1SEL0
	P3SEL1 |= BIT3;				// Setup pin P3.3 to output from the TA1 clock

	TA0CTL = TA0CTL | 0x0010;				// Start the clock
	TA1CTL = TA1CTL | 0x0010;				// Start the clock


	
	_BIS_SR(LPM0_bits | GIE); // Enter Low Power Mode 0 and activate interrupts
	while(1);
}


#pragma vector=PORT1_VECTOR
__interrupt void buttonPressed(void){
	//check which button

	if(P1IFG & 0x02){
		//btn 1.1
		TA0CTL = TA0CTL & ~0x0010;				// Stop the clock
		//TODO: adjust duty cycle - increment
		TA0CCR1 += STEP_SIZE;
	} else if(P1IFG & 0x04){
		//btn 1.2
		TA0CTL = TA0CTL & ~0x0010;				// Stop the clock
		TA0CCR1 -= STEP_SIZE;
		//TODO: adjust duty cycle - decrement

	} else if(P1IFG & 0x08){
		//btn 1.3
		//guessVal = C
	}
	if(TA0CCR0<4630){TA0CCR0 = 4630;}
	else if(TA0CCR0>18520){TA0CCR0 = 18520;}
	TA0CTL = TA0CTL | 0x0010;				// Start the clock
	//debounce?
	//Do thing

	P1IFG = P1IFG & (~BIT3);
	P1IFG = P1IFG & (~BIT2);
	P1IFG = P1IFG & (~BIT1);
}
