#include <msp430.h>
#define STOP_WATCHDOG 0x5A80 // Stop the watchdog timer
#define ACLK 0x0100 // Timer ACLK source
#define UP 0x0010 // Timer UP mode
#define ENABLE_PINS 0xFFFE // Required to use inputs and outputs
main() {
	WDTCTL = STOP_WATCHDOG; // Stop the watchdog timer
	PM5CTL0 = ENABLE_PINS; // Required to use inputs and outputs

	P1DIR = BIT0; // Set pin for red LED as an output
	P1OUT &= ~0x01; // Make sure red LED is off to start

	P1DIR = P1DIR & ~BIT1;     // P1.1 (Button S1) will be an input
	P1REN = P1REN | BIT1;      // P1.1 will have a pull-up
	P1OUT = P1OUT | BIT1;      // resistor.

	TA0CCR0 = 2000; // 40K*25us ~ 1 second to ISR
	TA0CTL = ACLK + UP; // Set ACLK, UP MODE
	TA0CCTL0 = CCIE; // Enable interrupt for Timer0

	_BIS_SR(LPM0_bits | GIE); // Enter Low Power Mode 0 and activate interrupts
	while(1);
}
//************************************************************************
// Timer0 Interrupt Service Routine
//************************************************************************
#pragma vector=TIMER0_A0_VECTOR
__interrupt void timerFinished(void){
	unsigned char k;
	unsigned char debounce = 0;
	unsigned int i;

	for (k = 0; k < 15; k++) {           // check buttons 15 times
		for (i = 0; i < 100; i++);      // delay to check buttons (~1ms)
		if (!(P1IN & 0x02))
			debounce++; // if pressed count
	} //end for(k)

	if (debounce > 5){   // pressed 5 of 15 times so a button is pressed ( P 1.2)
		P1OUT = P1OUT | BIT0; // turn the red LED on P1.0
	}else{
		P1OUT = P1OUT & ~BIT0; // turn the red LED off P1.0
	}

}
