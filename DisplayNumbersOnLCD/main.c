/* ECE 422 - Joe Leveille
 * Assignment 3 - Displaying numbers on LCD
 *
 */


#include <msp430.h>
#include <stdlib.h>
#include <string.h>
#include <driverlib.h> // Required for the LCD
#include "myGpio.h" // Required for the LCD
#include "myClocks.h" // Required for the LCD
#include "myLcd.h" // Required for the LCD
#include <time.h>
main() {
	signed long i = 0; // Number to be displayed
	unsigned long j = 0;              // For delay
	WDTCTL = 0x5A80;           // Disable WDT (poor doggy!)
	initGPIO();                    // Initializes Inputs and Outputs for LCD
	initClocks();                   // Initialize clocks for LCD
	myLCD_init();                   // Prepares LCD to receive commands
	while (1) {
		myLCD_displayNumber(i);     // Display the number
		i = i + 1;                    // Increment the number
		for (j = 0; j < 123456; j++);      // Delay
	}
}
}
