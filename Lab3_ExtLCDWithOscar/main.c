/*
 * Lab 3 - External LCD
 */
#include <msp430.h> 
#define SMCLK           0x0200          // Timer_A SMCLK source
#define RED_LED 0x0001 // P1.0 is the Red LED
#define STOP_WATCHDOG 0x5A80 // Stop the watchdog timer
#define ACLK 0x0100 // Timer ACLK source
#define UP 0x0010 // Timer Up mode
#define STOP 0x0030 // Timer Up mode
#define TACLR_ 0x0004 // Clear TAxR
#define SLOW 0x00C0 // Slow down the clock TAxR
#define ENABLE_PINS 0xFFFE // Required to use inputs and outputs

/*
 * TODO:
 * - function for specific timed delays delay(int microSecs)
 * - initialize LCD in 4bit
 * - clearScreen()
 * - setCursor(int location) //0 -> 31
 * - printToLCD(string word) //>16 chars or scroll
 * - setRow(int rowNum) //0 - 2, 2 being both
 * - fill main with demo
 * - LOW POWER MODE
 */
int main(void) {
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer
    PM5CTL0 = 0xFFFE; // Turn on the Digital I/o
//    P9DIR |= DB7 | ENABLE;		//Activate P9 outputs
//    P4DIR |= DB6 | DB5;		//Activate P4 outputs
//    P3DIR |= DB4;		//Activate P3 outputs
//    P1DIR |= 0x01 | RS;		//Activate P1 outputs

    extLCDinit();

    /*
     * 9.2 = DB7
     * 4.3 = DB6
     * 4.2 = DB5
     * 3.2 = DB4
     * 9.3 = ENable
     * 1.4 = RS
     *
     */
	
	return 0;
}
