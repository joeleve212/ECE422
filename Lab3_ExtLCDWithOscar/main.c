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
    extLCDinit();
    //TODO: pin assignments:
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
