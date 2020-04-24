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

void ScrollWords(char words[300]);
void task1();
void task2();
void task3();

typedef struct TCB{
	void (*functionPtr)(void); // Pointer to task function
	jmp_buf *task_buf; // Jump buffer
	char started; // Flag if task is started
	unsigned int taskData[800]; // Memory storage for SP, PC, etc...
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
	//TODO: fill in
}

void task2(){
	//TODO: fill in
}

void task3(){
	//TODO: fill in
}

