/*
 * Lab 3 - External LCD
 */
#include <msp430.h> 
#include "OB_JL_4bitLCD.h"
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
 * TODO: Things with stars in front are not completed yet
  - function for specific timed delays delay(int milliSecs)
  - initialize LCD in 4bit
  - clearScreen()
  - setCursor(int location) //0 -> 31
 * - printToLCD(string word) //>16 chars or scroll			TODO: handle strings that will spill over
 * - setRow(int rowNum) //0 - 2, 2 being both
  - fill main with demo
  - LOW POWER MODE
 * - Extra features
 */

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer
    PM5CTL0 = 0xFFFE; // Turn on the Digital I/o

    extLCDinit();
    /*setCursor(3);
    printToLCD("asdf");
    setCursor(16);
    printToLCD("test");

    setCursor(24);
    printToLCD("yeet");*/

    setCursor(16);
    printToLCD("The quick brown ");

    delay(4000);
    clearScreen();

    printToLCD("The quick brown fox jumps ");
    /*
     * 9.2 = DB7
     * 4.3 = DB6
     * 4.2 = DB5
     * 3.2 = DB4
     * 9.3 = ENable
     * 1.4 = RS
     *
     */
    _BIS_SR(LPM0_bits);
	while(1);
}
