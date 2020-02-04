#include <msp430.h> 
#include "mylib.h"
#include <string.h>
#include <math.h>
#include "driverlib/MSP430FR5xx_6xx/driverlib.h" // Required for the LCD
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

#define SMCLK           0x0200          // Timer_A SMCLK source
#define RED_LED 0x0001 // P1.0 is the Red LED
#define STOP_WATCHDOG 0x5A80 // Stop the watchdog timer
#define ACLK 0x0100 // Timer ACLK source
#define UP 0x0010 // Timer Up mode
#define STOP 0x0030 // Timer Up mode
#define TACLR_ 0x0004 // Clear TAxR
#define SLOW 0x00C0 // Slow down the clock TAxR
#define ENABLE_PINS 0xFFFE // Required to use inputs and outputs

void ScrollWords(char words[250]);//Scroll across the words
void SubmitButton();
int rowNum,index=0,currNum=0;
int digs[3];  //-1 means not filled with a value

//is the multiplier currently showing a result
int currently_on = 0;
int prev = 0, curr = 0;//the previous results of the the

int still_pressed = 0;

int cancel_prompt;

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

#define STAR_KEY 10
#define POUND_KEY 12

int test_all(){
	int row, btnVal=0;
    for(row = 1;row<5;row++){
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
    if (btnVal == 11) btnVal = 0;
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
    P1IFG = P1IFG & (~BIT1); //clear P1.1 flag

	MPY32CTL0 = 0x0200; //setup multiplier
    currently_on = 0;//the multiplier is not currently on
    prev = 0;
    curr = 0;

    TA0CCR1 = 9600;		//set timer for quarter second   19230 = up time for 1Hz @ 50%, 38 = up time for 999Hz @ 50%
    TA0CCR0 = 9600;                     // the time on
    TA0CTL = ACLK + UP + TACLR;
    TA0CCTL1 = 0x00E0; // Setup TA0 to output in Reset/Set mode

    P1SEL0 |= 0x01; // Setup pin P1.0 to output from the TA0 clock
    P1SEL1 &= ~0x01; // Setup pin P1.0 to output from the TA0 clock
    still_pressed = 0;
    TA1CCR0 = 23437; // Sets value of Timer_0
    TA1CTL = ACLK + SLOW + TACLR_; // Start TA0 from zero with ACLK in UP MODE
    TA1CCTL0 = CCIE; // Enable interrupt for Timer_0

    _BIS_SR(GIE); // Activate interrupts

    ScrollWords("USE THE KEYPAD TO ENTER A FREQUENCY      PRESS THE POUND BUTTON TO SUBMIT OR THE DASH BUTTON FOR BACKSPACE      THE RED LED WILL OUTPUT YOUR INPUT FREQUENCY      AFTER FIVE SECONDS THE LCD SCREEN WILL DISPLAY THE MULTIPLICATION OF THE LAST TWO FREQUENCIES");

    _BIS_SR(LPM0_bits | GIE); // Enter Low Power Mode 0 and activate interrupts
    return 0;
}

#pragma vector=PORT2_VECTOR
__interrupt void Port_2(void) {
    cancel_prompt = 1;
    if (still_pressed) {
        if((P2IN & COL1) && (P2IN & COL2) && (P2IN & COL3)){//is the port not pressed
            //the port is not preessed
            if (P2IV); // must read port interrupt vector to reset the highest pending interrupt
            P2IFG ^=  P2IFG;
            still_pressed = 0;
        }
        return;
    }
    //debounce
    int i;
    for(i = 0;i<100;i++);
    if((P2IN & COL1) && (P2IN & COL2) && (P2IN & COL3)){//is the port not pressed
        //the port is not preessed
        if (P2IV); // must read port interrupt vector to reset the highest pending interrupt
        P2IFG ^=  P2IFG;
        still_pressed = 0;
    }
    int btn_pressed = test_all();

    if (btn_pressed<10){//if it is an integer
    	digs[2]=digs[1];
    	digs[1]=digs[0];
    	digs[0]= btn_pressed;
        myLCD_displayNumber(interpNum(digs));
    } else if (btn_pressed == STAR_KEY){
        SubmitButton();
    } else if (btn_pressed == POUND_KEY){
        digs[0]= digs[1];
        digs[1]= digs[2];
        digs[2]=0;
        myLCD_displayNumber(interpNum(digs));
    }
	if((P2IN & COL1) && (P2IN & COL2) && (P2IN & COL3)){
        if (P2IV); // must read port interrupt vector to reset the highest pending interrupt
        P2IFG ^=  P2IFG;
	}else{
        still_pressed = 1;
	}
}
long int val;
void start_LCD_Multiply(int new_freq){
    prev = curr;
    curr = new_freq;
    val = multiply(prev,curr);
    currently_on = 0;
    TA1R = 0x00;
    TA1CTL |= UP;
}

#pragma vector=TIMER1_A0_VECTOR
__interrupt void Timer0_ISR(void) {
    if (!currently_on){
        myLCD_displayNumber(val);
        currently_on = 1;
    }else{
        myLCD_displayNumber(interpNum(digs));
        TA1CTL &= ~STOP;
        currently_on = 0;
    }
}
void SubmitButton(){
    int dispVal=interpNum(digs);
    if (dispVal == 0) return;
    int period = 0;
    if (dispVal>100){//improve readability
        TA0CTL = SMCLK + UP + TACLR;
        period = (int)(1000000.0 /(float)dispVal);
    }else{
        TA0CTL = ACLK + UP + TACLR;
        period = (int)(37500.0 /(float)dispVal);
    }
    TA0CCR0 = period;
    TA0CCR1 = period/2;

    start_LCD_Multiply(interpNum(digs));

    digs[0]= 0;
    digs[1]= 0;
    digs[2]=0;
}

#pragma vector=PORT1_VECTOR
__interrupt void SubmitButtonInterupt(void){
    cancel_prompt = 1;
    SubmitButton();
    P1IFG = P1IFG & (~BIT1); //clear P1.1 flag

}

//********************************************************************************************
// ScrollWords()
//********************************************************************************************
void ScrollWords(char words[300]) {
    unsigned int length; // Contains length of message to be displayed
    unsigned int slot; // Slot to be displayed on LCD (1, 2, 3, 4,
    // 5, or 6)
    unsigned int amount_shifted; // Number of times message shifted so far
    unsigned int offset; // Used with amount_shifted to get correct
    // character to display
    unsigned long delay; // Used to implement delay between scrolling
    // iterations
    unsigned char next_char; // Next character from message to be
    // displayed
    length = strlen(words); // Get length of the message stored in words
    amount_shifted = 0; // We have not shifted the message yet
    offset = 0; // There is no offset yet

    cancel_prompt = 0;

    while ((amount_shifted < length + 7)&&(!cancel_prompt)){ // Loop as long as you haven't shifted all
    														// of the characters off the LCD screen
        offset = amount_shifted; // Starting point in message for next LCD update
        for (slot = 1; slot <= 6; slot++) // Loop 6 times to display 6 characters at a time
                {
            next_char = words[offset - 6]; // Get the current character for LCD slot
            if (next_char && (offset >= 6) && (offset <= length + 6)) // If character is not null AND
                    { // LCD is not filled (offset>=6) AND
                      // You have not reached end of message
                      // (offset<=length+6)
                myLCD_showChar(next_char, slot); // Show the next character on the LCD
                // screen in correct slot
            }  //end if
            else // Else, slot on LCD should be blank
            {
                myLCD_showChar(' ', slot); // So, add a blank space to slot
            } //end else
            offset++; // Update as you move across the message
        } //end for
        for (delay = 0; delay < 63456; delay = delay + 1)
            ; // Delay between shifts
        amount_shifted = amount_shifted + 1; // Update times words shifted across LCD
    } //end while
}
