#include <msp430.h> 
#define ROW1 BIT0
#define ROW2 BIT1
#define ROW3 BIT5
#define ROW4 BIT6
#define COL1 BIT0
#define COL2 BIT1
#define COL3 BIT2

#define RED_LED 0x0001 // P1.0 is the Red LED
#define STOP_WATCHDOG 0x5A80 // Stop the watchdog timer
#define ACLK 0x0100 // Timer ACLK source
#define UP 0x0010 // Timer Up mode
#define TACLR_ 0x0004 // Clear TAxR
#define ENABLE_PINS 0xFFFE // Required to use inputs and outputs

int rowNum;

void setAllLow(){
    P9OUT = P9OUT & ~( ROW1 | ROW2 | ROW3| ROW4); //Turn off the rows
}

void setUpRails(){
    PM5CTL0 = 0xFFFE; // Turn on the Digital I/o
    P1DIR = 0x01;

    P9DIR = P9DIR | ROW1 | ROW2 | ROW3| ROW4;  // Set up the
    P9OUT = P9OUT | ROW1 | ROW2 | ROW3| ROW4; //Turn on the rows

    //input
    P3DIR = P3DIR & ~(COL1|COL2|COL3); // P1.1 and P1.2 will be inputs
    P3REN = P3REN | COL1|COL2|COL3; // P1.1 and P1.2 will have pull-up
    P3OUT = P3OUT | COL1|COL2|COL3; // resistors

    setAllLow();

    P3IE = COL1|COL2|COL3; // Enable interrupt ONLY for P1.1 and P1.2
    P3IES = COL1|COL2|COL3; // for transitions from HI-->LO
    P3IFG = 0x00; // Ensure no interrupts are pending



}

void setLowRail(int n){
    P9OUT = P9OUT | ROW1 | ROW2 | ROW3| ROW4; //Turn on the rows
    switch (n)
    {
        case 1: // code to be executed if n = 1;
            P9OUT = P9OUT & ~ROW1; //Set row to low
            P9OUT |= ROW2|ROW3|ROW4; //Set rows to high
            break;
        case 2: // code to be executed if n = 2;
            P9OUT = P9OUT & ~ROW2; //Set row to low
            P9OUT |= ROW1|ROW3|ROW4; //Set rows to high
            break;
        case 3: // code to be executed if n = 3;
            P9OUT = P9OUT & ~ROW3; //Set row to low
            P9OUT |= ROW1|ROW2|ROW4; //Set rows to high
            break;
        case 4: // code to be executed if n = 4;
            P9OUT = P9OUT & ~ROW4; //Set row to low
            P9OUT |= ROW2|ROW3|ROW1; //Set rows to high
            break;
        default: // code to be executed if n doesn't match any cases
            break;
    }
}

int test_all(){	//TODO: FIX triggering multiple low rail cases
	int row;
    for(row = 1;row<=4;row++){
    	setLowRail(row);
		//need to identify which button.
		if (!(P3IN & COL1)){
			P1OUT |= 0x01;
			return row;
		}else if (!(P3IN & COL2)){
			P1OUT &= ~0x01;
			return row;
		}else if (!(P3IN & COL3)){
			P1OUT ^= 0x01;
			return row;
		}
    }


//    setLowRail(2);
//    if (!(P3IN & COL1)){
//        P1OUT |= 0x01;
//    }else if (P3IN & COL2){
//        P1OUT &= ~0x01;
//    }else if (P3IN & COL3){
//        P1OUT ^= 0x01;
//    }
//
//    setLowRail(3);
//    if (!(P3IN & COL1)){
//        P1OUT |= 0x01;
//    }else if (P3IN & COL2){
//        P1OUT &= ~0x01;
//    }else if (P3IN & COL3){
//        P1OUT ^= 0x01;
//    }
//
//    setLowRail(4);
//    if (!(P3IN & COL1)){
//        P1OUT |= 0x01;
//    }else if (P3IN & COL2){
//        P1OUT &= ~0x01;
//    }else if (P3IN & COL3){
//        P1OUT ^= 0x01;
//    }

    setAllLow();
    return -1;
}

/*
int no_input = 0;
int rail_off = 0;
int last_button_pressed = -1;
void switch_rails(){
    rail_off+=1;
    if(rail_off>4){rail_off=0;}
    setLowRail(rail_off);
}
*/


/*
 * main.c
 */
int main(void) {
    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer

    setUpRails();


    //TA0CCR0 = 200; // Sets value of Timer_0
    //TA0CTL = ACLK + UP + TACLR_; // Start TA0 from zero with ACLK in UP MODE

    //TA0CCTL0 = CCIE; // Enable interrupt for Timer_0

    _BIS_SR(LPM0_bits | GIE); // Enter Low Power Mode 0 and activate interrupts

    return 0;
}


#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer0_ISR(void) {
}

#pragma vector=PORT3_VECTOR
__interrupt void Port_3(void) {
    rowNum = test_all();

    if (P3IV)
        ; // must read port interrupt vector to reset the highest pending interrupt
    P3IFG ^=  P3IFG;
}


