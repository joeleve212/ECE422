//************************************************************************
// void Task2(void) Function
//
// Uses Timer TA2 to create a 1 HZ, 50% duty cycle
// signal on P9.7 (Green LED) and also every three
// seconds checks a global variable (HoldGreenLED)
// and will hold P9.7 high for the number of seconds
// corresponding to the interger contained in HoldGreenLED. 
// Afterward, the 1 Hz 50% duty cycle signal resumes.  
//************************************************************************
//
// Phil Walter
// 03/18/2020  v1
// for CE-422, Project 5
//************************************************************************
#include <msp430.h>
int HoldGreenLED;
void Task2(void){
// Setup - runs once
	unsigned char count = 0;   // local variable

	P9DIR |= BIT7;       // make P9.7 (Green LED) an output      
	P9OUT &= ~BIT7;      // Turn off P9.7 (Green LED)

	TA2CCR0 = 16384;     // setup TA2 timer to count for 0.5 second
	TA2CTL = 0x0114;     // start TA2 timer from zero in UP mode with ACLK 

// loop that repeats forever - 1Hz 50% DC signl or on for HoldGreenLED seconds
	while(1){             // infinite loop
// 1 Hz 50% DC signal
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
