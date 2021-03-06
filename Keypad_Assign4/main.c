/* Joe Leveille
 * ECE 422 Assignment 4 - Assembly + C
 */

#include <msp430.h> 
#include "mylib.h"

unsigned long int var1=3, var2=4;
int sum, btnPres = 0;

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;		// Stop watchdog timer
    PM5CTL0 = 0xFFFE; 				//activate I/O pins
	P1DIR |= BIT0; 					// Set RED LED to output
	P9DIR |= BIT7; 					// Set Green LED to output
	P1OUT &= ~BIT0;					//turn LEDs off
	P9OUT &= ~BIT7;
	
	P1DIR = P1DIR & ~BIT1; 			// P1.1 (Button S1) will be an input
	P1REN = P1REN | BIT1;			// P1.1 will have a pull-up
	P1OUT = P1OUT | BIT1; 			// resistor.

	P1DIR = P1DIR & ~BIT2; 			// P1.2 (Button S1) will be an input
	P1REN = P1REN | BIT2;			// P1.2 will have a pull-up
	P1OUT = P1OUT | BIT2; 			// resistor.

	P1IE |= BIT1; 					// Enable interrupt ONLY for P1.1
	P1IES |= BIT1; 					// for transitions from HI-->LO
	P1IE |= BIT2; 					// Enable interrupt ONLY for P1.2
	P1IES |= BIT2; 					// for transitions from HI-->LO

	P1IFG ^= P1IFG; 				//Clear button flags
	int i;
	_BIS_SR(GIE); 					// Enter Low Power Mode 0 and activate interrupts
	while(1){
		comp(var1, var2);
		for(i=0;i<20000;i++);
	}
}

#pragma vector=PORT1_VECTOR
__interrupt void buttonPressed(void){
	if(P1IFG & 0x02){ 				//P1.1 decrement var1
		var1--;
	} else if(P1IFG & 0x04){ 		//P1.2 increment var1
		var1++;
	}
	P1IFG ^= P1IFG; 				//Clear button flags
	if(P1IV);
}
