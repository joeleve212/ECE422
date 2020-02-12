/*
 * Lab 3 - External LCD
 */
#include <msp430.h> 

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
	
	return 0;
}
