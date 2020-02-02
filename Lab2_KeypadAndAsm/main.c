#include <msp430.h> 
#include "mylib.h"
#include <string.h>
#include <math.h>
#include <driverlib.h> // Required for the LCD
#include "myGpio.h" // Required for the LCD
#include "myClocks.h" // Required for the LCD
#include "myLcd.h" // Required for the LCD
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

int rowNum,index=0,currNum=0;
long int val;
int digs[3];  //-1 means not filled with a value

void setAllLow(){
    P9OUT &= ~( ROW1 | ROW2 | ROW3| ROW4); //Turn off the rows
}

void setUpRails(){
    PM5CTL0 = 0xFFFE; // Turn on the Digital I/o
    P1DIR |= 0x01;		//Activate P1.0 as Output

    P9DIR |= ROW1 | ROW2 | ROW3| ROW4;  // Set up row pins as outputs
    P9OUT |= ROW1 | ROW2 | ROW3| ROW4; //Turn on the rows

    //input
    P2DIR &= ~(COL1|COL2|COL3); // P1.1 and P1.2 will be inputs
    P2REN |= COL1|COL2|COL3; // P1.1 and P1.2 will have pull-up
    P2OUT |= COL1|COL2|COL3; // resistors

    setAllLow();

    P2IE |= COL1|COL2|COL3; // Enable interrupt ONLY for P2.0,P2.1,P2.2
    P2IES |= COL1|COL2|COL3; // for transitions from HI-->LO
    P2IFG = 0x00; // Ensure no interrupts are pending



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

int test_all(){
	int row, btnVal=0;
    for(row = 1;row<4;row++){
    	setLowRail(row);
		//need to identify which button.
		if (!(P2IN & COL1)){
			btnVal = 3*(row-1)+1;
			break;
		}else if (!(P2IN & COL2)){
			btnVal = 3*(row-1)+2;
			break;
		}else if (!(P2IN & COL3)){
			btnVal = 3*(row-1)+3;
			break;
		}
    }

    setAllLow();

    return btnVal; //gives back value (bottom row is all interpreted as 0s)
}

int interpNum(int digits[]){
	int enteredVal = 0;
	int i;
	for(i=0;i<3;i++){//index 0 is ones digit
		if(digs[i]<0){break;}
		enteredVal += pow(10,i) * digs[i];
	}
	return enteredVal;
}

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer
    initGPIO(); // Initialize General Purpose Inputs and Outputs for the LCD
	initClocks(); // Initialize clocks for the LCD
	myLCD_init(); // Initialize Liquid Crystal Display
    setUpRails();
    digs[0]=-1;
    digs[1]=-1;
    digs[1]=-1;
    P1IE |= BIT1; 							// Enable interrupt ONLY for P1.1
	P1IES |= BIT1; 							// for transitions from HI-->LO

    TA0CCR0 = 9600;		//set timer for quarter second   19230 = up time for 1Hz @ 50%, 38 = up time for 999Hz @ 50%
	TA0CTL = ACLK + UP + TACLR;
	TA0CCTL0 = CCIE; // Enable interrupt for Timer_0
	TA0CTL &= ~0x0010; //stop clock for now

    _BIS_SR(LPM0_bits | GIE); // Enter Low Power Mode 0 and activate interrupts
    return 0;
}


#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer0_ISR(void) {
//	int prev = 2, curr = 5;
//	val = multiply(prev,curr);//call the assembly function to multiply the values
//	myLCD_displayNumber(val);
	P1OUT ^= 0x01;
	//TODO: use timer to trigger 50% duty @ freq = entered value
	//TODO: timer for 5 second delay till multiplication is shown
}

#pragma vector=PORT2_VECTOR
__interrupt void Port_2(void) {


//    if(digs[index]==-1){index = 3;} //increment
//    else{
    	digs[2]=digs[1];
    	digs[1]=digs[0];
    	digs[0]= test_all();
//    }
    int prev = 2, curr = 5;
	val = multiply(prev,curr);
	myLCD_displayNumber(interpNum(digs));
    if (P2IV); // must read port interrupt vector to reset the highest pending interrupt
    P2IFG ^=  P2IFG;
}

#pragma vector=PORT1_VECTOR
__interrupt void SubmitButton(void){
	//TODO: handle btn 1.1 to submit freq value/start timers
	//19230 = up time for 1Hz @ 50%, 38 = up time for 999Hz @ 50%
	int dispVal=interpNum(digs);

	TA0CCR0 = (int)(((float)dispVal/999.0)*(19230-38))+38;
	//TA0CTL |= 0x0010;				// Start the clock
	P1IFG = P1IFG & (~BIT1); //clear P1.1 flag
}

