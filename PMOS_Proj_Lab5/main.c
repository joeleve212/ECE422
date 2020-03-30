#include <msp430.h> 

#include <stdlib.h>
#include <string.h>
#include <driverlib.h> // Required for the LCD
#include "myGpio.h" // Required for the LCD
#include "myClocks.h" // Required for the LCD
#include "myLcd.h" // Required for the LCD
#include <time.h>

int HoldGreenLED;
int main(void) {
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer




	//TODO: init array (length of max num/tasks) of structs
    //init each task w/ it's struct in corresponding loc in array
    //
	return 0;
}
void task1(){
	initGPIO();                // Initializes Inputs and Outputs for LCD
	initClocks();              // Initialize clocks for LCD
	myLCD_init();              // Prepares LCD to receive commands
	P1DIR = P1DIR & ~BIT1;     // P1.1 (Button S1) will be an input
	P1REN = P1REN | BIT1;      // P1.1 will have a pull-up
	P1OUT = P1OUT | BIT1;      // resistor.
	
	while(1){
		//loop TODO: display counter(start at 0) on LCD
		if(P1IN & 0x02 || counter == 999999){ //check if P1.1 is pressed
			//reset counter to 0
			//display "START ", then " OVER "
		}
		//TODO: wait 1 sec
		//TODO:increment counter
	}
}
void task2(void){ // Setup - runs once
	unsigned char count = 0;   // local variable
	P9DIR |= BIT7;       // make P9.7 (Green LED) an output
	P9OUT &= ~BIT7;      // Turn off P9.7 (Green LED)
	TA2CCR0 = 16384;     // setup TA2 timer to count for 0.5 second
	TA2CTL = 0x0114;     // start TA2 timer from zero in UP mode with ACLK
	// loop that repeats forever - 1Hz 50% DC signl or on for HoldGreenLED seconds
	while(1){     // infinite loop// 1 Hz 50% DC signal
		if (TA2CTL & BIT0){     // check if timer finished counting
			TA2CTL &= ~BIT0;     // reset timer flag
			P9OUT ^= BIT7;       // toggle P9.7 (Green LED)
			count++;             // increment counter
		}//end if (TA2CTL & BIT0)
		// every three seconds get HoldGreenLED value and make P9.7 high
		if (count >= 6){        // 3 seconds elapsed?
			TA2CCR0 = 32768;     // set timer to count for 1 second
			TA2CTL = 0x0114;     // start TA2 timer from zero in UP mode with ACLK
			P9OUT |= BIT7;       // Turn On P9.7 (Green LED)
			count = HoldGreenLED;     // set local variable equal to global variable
                                // how many seconds to keep P9.7 high
			while (count > 0){         // count down to zero
				if (TA2CTL & BIT0){      // check if timer done counting to 1 second
					TA2CTL &= ~BIT0;      // reset timer flag
					count--;              // decrement counter
				}
			}//end while(count > 0)
// Done with P9.7 high, go back to 1 Hz 50% DC signal
			TA2CCR0 = 16384;      // setup TA2 timer to count for 0.5 second
			TA2CTL = 0x0114;      // start TA2 timer from zero in UP mode with ACLK
			P9OUT ^= BIT7;        // start with P9.7 (Green LED) off
		}//end if (count >= 6)
	}//end while(1)
}//end Task2()

void task3(){
	//setup TODO:

	while(1){
		//Loop TODO:
		//wait 10 seconds
		//randomize value 0 - 25
		//place value in holdGreenLED
		//mess with P1.0
	}
}
