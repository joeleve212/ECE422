#include <msp430.h>
#include <setjmp.h>
#include <stdlib.h>
#include <string.h>
#include <driverlib.h> // Required for the LCD
#include "myGpio.h" // Required for the LCD
#include "myClocks.h" // Required for the LCD
#include "myLcd.h" // Required for the LCD
#include <time.h>
#define ENABLE_PINS 0xFFFE // Required to use inputs and outputs
#define maxNumTasks 3

int HoldGreenLED;
char currTask = 1;
char priority[3] = {0, 0, 1};     //set the priority of each task (only task 3 is priority)
char p1Card='K',p2Card='4',p1Suit='C',p2Suit='D';
short drawFlag = 0, printNow = 0;

void ScrollWords(char words[300]);
void task1();
void task2();
void task3();

typedef struct TCB{
	void (*functionPtr)(void); // Pointer to task function
	jmp_buf *task_buf; // Jump buffer
	char started; // Flag if task is started
	unsigned int taskData[1000]; // Memory storage for SP, PC, etc... (use 800 - 1000 for vars)
}TCB;

#pragma PERSISTENT(taskBlocks);
jmp_buf taskRegs[maxNumTasks];
TCB taskBlocks[maxNumTasks] = { //Setup Task Control Block array
	{task1, taskRegs[0], 0, 0},
	{task2, taskRegs[1], 0, 0},
	{task3, taskRegs[2], 0, 0}  //not started, no data
};

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer
    PM5CTL0 = ENABLE_PINS; 				// Enable inputs and outputs
    HoldGreenLED = 3; // place some default value for the LED

    //TODO: use ScrollWords to show directions
    //TODO: explain order of suits: H, D, C, S

    TA0CCR0 = 10000;     // setup TA0 timer to count for default length of time
    TA0CTL = 0x0114;     // start TA0 timer from zero in UP mode with ACLK
    TA0CCTL0 = CCIE; // Enable interrupt for Timer0
    _BIS_SR(GIE); // Enter activate interrupts
	return 0;
}
#pragma vector=TIMER0_A0_VECTOR
__interrupt void OStimer(void){ //handle task incrementing
	TA0CTL = TA0CTL & ~TAIFG;    //reset ISR flag
	static const int defaultTime = 1000, longTime = 2000; //set time lengths

	if(!setjmp(taskBlocks[currTask].task_buf)){ // Save jmp 'Check point'
		currTask = ++currTask % 3; //cycle to the next task

		if(priority[currTask]==1){
			TA0CCR0 = longTime;     // setup TA0 timer to count for long time length
		} else{
			TA0CCR0 = defaultTime;     // setup TA0 timer to count for default time length
		}

		if(taskBlocks[currTask].started){  				// Check if current task has started
			longjmp(taskBlocks[currTask].task_buf,1);	// Jump to next task if already started
		} else {
			taskBlocks[currTask].started = true; 		// Mark Task as started
			__set_SP_register(taskBlocks[currTask].taskData + 800); // Load next task data into SP
			_BIS_SR(GIE); 								// Reactivate TA0 interrupt
			(taskBlocks[currTask].functionPtr)(); 		// Jump into next task
		}
	}
}

void task1(){
	P1DIR = P1DIR & ~BIT1;     // P1.1 (Button S1) will be an input
	P1REN = P1REN | BIT1;      // P1.1 will have a pull-up
	P1OUT = P1OUT | BIT1;      // resistor.

	while(1){
		//TODO: add debounce?
		if(P1IN & 0x02){ //check if button P1.1 is pressed
			drawFlag = 1; //Set draw flag = 1, store that in taskData[800]

			while(drawFlag == 1){ //spin here until task switch
			}
		} else{
			drawFlag = 0; //clear draw flag
		}
	}
}

drawCards(){
	//
}

void task2(){
	//TODO: setup vars & randomization

	while(1){
		if(drawFlag==1){ //check if draw is needed
			//TODO: trigger drawing suit and card, prevent double

			printNow = 1; //set ready to print flag
		} else{
			//TODO: if not, return? or go to next? or keep checking?
			printNow = 0; //clear ready to print flag
			while(printNow == 0){ //spin here until task switch
			}
		}
	}
}

void task3(){
	initGPIO();                // Initializes Inputs and Outputs for LCD
	initClocks();              // Initialize clocks for LCD
	myLCD_init();              // Prepares LCD to receive commands

	myLCD_showChar(p1Card,1);
	myLCD_showChar(p1Suit,2);
	myLCD_showChar(p2Card,5);
	myLCD_showChar(p2Suit,6);
	while(1){
		if(printNow == 1){ //if ready to print
			//TODO: print cards
			myLCD_showChar(p1Card,1);
			myLCD_showChar(p1Suit,2);
			myLCD_showChar(p2Card,5);
			myLCD_showChar(p2Suit,6);
			printNow = 0; //clear flag
		} else {
			while(printNow == 0){ //spin here until task switch
			}
		}
	}

}

