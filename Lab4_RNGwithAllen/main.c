#include <msp430.h> 

/*
 * main.c
 */
int main(void) {
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer
	P2DIR |= 0x0F;  // Make 2.0 - 2.3 outputs
	P2OUT &= ~0x0F; // Make 2.0 - 2.3 = 0
	P2OUT |= 0x07; //display 7?
	
	return 0;
}
