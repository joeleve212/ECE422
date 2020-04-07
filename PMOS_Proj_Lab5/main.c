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

int HoldGreenLED,dummy,startUp = 1,num=100;
int currTask = 1;
jmp_buf taskRegs[maxNumTasks+1];
struct timer{
	int stopCount, currCount, controlReg;
};

struct taskRegs{ //TODO: This should be uneeded with longjump() & setjmp()
	//TODO: fill with registers that must be saved
	int progCounter,status,genRegs;
	struct timer timers[3];

};
void ScrollWords(char words[300]);
void task1();
void task2();
void task3();
void somethingInteresting(){ //Randomly turns P1.0 on or off 20 times based on TA3 count
	int i,j,onBool;
	for(j=0;j<20;j++){
		onBool = rand() % 2; //decides whether to turn LED on or off first
		for(i=0;i<200;i++);
		if(onBool){ //if odd number,
			P1OUT |= 0x01; //turn on LED
		} else{ //if even,
			P1OUT &= ~0x01; //turn off LED
		}
		dummy = setjmp(taskRegs[currTask]); //Save current task's regs in taskRegs[currTask]          ------------------
	}
	return;
}

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer
    PM5CTL0 = ENABLE_PINS; 				// Enable inputs and outputs
    HoldGreenLED = 3; // place some default value for the LED
    TA0CCR0 = 100000;     // setup TA0 timer to count for 100 ms
    TA0CTL = 0x0214;     // start TA0 timer from zero in UP mode with SMCLK
    TA0CCTL0 = CCIE; // Enable interrupt for Timer0
    _BIS_SR(GIE); // Enter activate interrupts
    //struct taskRegs sysTasks[maxNumTasks]; //this OS can handle at most 4 tasks
	//TODO: init array (length of max num/tasks) of structs OR import functions to store regs
    //init each task w/ it's struct in corresponding loc in array
    dummy = setjmp(taskRegs[0]); //Save current task's regs in taskRegs[currTask]          ------------------

    if(startUp){
    	switch(currTask){ //call appropriate current task
			case 1:
				task1();
				break;
			case 2:
				task2();
				break;
			case 3:
				startUp = 0;
				task3();
				break;
			default:
				break;
		}
    } else{
    	//TODO: start currTask
    	switch(currTask){ //call appropriate current task
			case 1:
				longjmp(taskRegs[currTask],num);
				break;
			case 2:
				longjmp(taskRegs[currTask],num);
				break;
			case 3:
				longjmp(taskRegs[currTask],num);
				break;
			default:
				break;
		}
    }


	return 0;
}
#pragma vector=TIMER0_A0_VECTOR
__interrupt void OStimer(void){ //TODO: handle task switching
	//dummy = setjmp(taskRegs[currTask]); //Save current task's regs in taskRegs[currTask]
	TA0CTL = TA0CTL & ~TAIFG;  //reset ISR flag
	TA0CCTL0 &= ~0x01; //reset CCR0 ISR flag
	if(currTask<3){
		currTask++;
	} else{
		currTask = 1;
	}
	//TODO: handle priority tasks
	return;
}

void task1(){
	dummy = setjmp(taskRegs[currTask]); //Save current task's regs in taskRegs[currTask]          -------------------------
	initGPIO();                // Initializes Inputs and Outputs for LCD
	initClocks();              // Initialize clocks for LCD
	dummy = setjmp(taskRegs[currTask]); //Save current task's regs in taskRegs[currTask]          -------------------------
	myLCD_init();              // Prepares LCD to receive commands
	P1DIR = P1DIR & ~BIT1;     // P1.1 (Button S1) will be an input
	dummy = setjmp(taskRegs[currTask]); //Save current task's regs in taskRegs[currTask]          -------------------------
	P1REN = P1REN | BIT1;      // P1.1 will have a pull-up
	P1OUT = P1OUT | BIT1;      // resistor.
	int counter = 0;
	TA1CCR0 = 32768;     // setup TA1 timer to count for 1 second
	TA1CTL = 0x0114;     // start TA1 timer from zero in UP mode with ACLK
	if(currTask != 1){
		longjmp(taskRegs[0],num);
	}
	dummy = setjmp(taskRegs[currTask]); //Save current task's regs in taskRegs[currTask]          ------------------
	while(1){
		if(currTask != 1){
			longjmp(taskRegs[0],num);
		}
		dummy = setjmp(taskRegs[currTask]); //Save current task's regs in taskRegs[currTask]         ---------------
		//LOOP display counter(start at 0) on LCD
		if(!(P1IN & 0x02) || counter == 999999){ //check if P1.1 is pressed
			counter = 0; //reset counter to 0
			ScrollWords("START OVER"); //display "START OVER"
		} else if(TA1CTL & BIT0){// check if timer finished counting
			TA1CTL &= ~BIT0;     // reset timer flag
			counter++; //increment counter
			myLCD_displayNumber(counter);
			TA1CCR0 = 32768;     // setup TA1 timer to count for 1 second
			TA1CTL = 0x0114;     // start TA1 timer from zero in UP mode with ACLK
		}
	}
}
void task2(void){ // Setup - runs once
	unsigned char count = 0;   // local variable
	P9DIR |= BIT7;       // make P9.7 (Green LED) an output
	P9OUT &= ~BIT7;      // Turn off P9.7 (Green LED)
	TA2CCR0 = 16384;     // setup TA2 timer to count for 0.5 second
	TA2CTL = 0x0114;     // start TA2 timer from zero in UP mode with ACLK
	if(currTask != 2){
		longjmp(taskRegs[0],num);
	}
	dummy = setjmp(taskRegs[currTask]); //Save current task's regs in taskRegs[currTask]          -------------------------
	// loop that repeats forever - 1Hz 50% DC signl or on for HoldGreenLED seconds
	while(1){     // infinite loop// 1 Hz 50% DC signal
		if(currTask != 2){
			longjmp(taskRegs[0],num);
		}
		dummy = setjmp(taskRegs[currTask]); //Save current task's regs in taskRegs[currTask]              -------------------
		if (TA2CTL & BIT0){     // check if timer finished counting
			TA2CTL &= ~BIT0;     // reset timer flag
			P9OUT ^= BIT7;       // toggle P9.7 (Green LED)
			count++;             // increment counter
		}//end if (TA2CTL & BIT0)
		// every three seconds get HoldGreenLED value and make P9.7 high
		if (count >= 6){        // 3 seconds elapsed?
			TA2CCR0 = 32768;     // set timer to count for 1 second
			TA2CTL = 0x0114;     // start TA2 timer from zero in UP mode with ACLK
			P9OUT |= BIT7;       // Turn On P9.7 (Green LED)
			count = HoldGreenLED;     // set local variable equal to global variable
                                // how many seconds to keep P9.7 high

			while (count > 0){         // count down to zero
				if (TA2CTL & BIT0){      // check if timer done counting to 1 second
					TA2CTL &= ~BIT0;      // reset timer flag
					count--;              // decrement counter
				}
				if(currTask != 2){
					longjmp(taskRegs[0],num);
				}
				dummy = setjmp(taskRegs[currTask]); //Save current task's regs in taskRegs[currTask]              -------------------
			}//end while(count > 0)
// Done with P9.7 high, go back to 1 Hz 50% DC signal
			TA2CCR0 = 16384;      // setup TA2 timer to count for 0.5 second
			TA2CTL = 0x0114;      // start TA2 timer from zero in UP mode with ACLK
			P9OUT ^= BIT7;        // start with P9.7 (Green LED) off
		}//end if (count >= 6)
	}//end while(1)
}//end Task2()

void task3(){
	//setup
	P1DIR |= BIT0;       // make P1.0 (Red LED) an output
	P1OUT &= ~BIT0;      // Turn off P1.0 (Red LED)
	TA3CCR0 = 32768;     // setup TA3 timer to count for 1 second
	TA3CTL = 0x0114;     // start TA3 timer from zero in UP mode with ACLK
	srand(time(NULL));   //seed randomizer
	int counter = 0; //start counter at 0
	if(currTask != 3){
		longjmp(taskRegs[0],num);
	}
	dummy = setjmp(taskRegs[currTask]); //Save current task's regs in taskRegs[currTask]                   -------------------
	while(1){ //Loop
		if(currTask != 3){
			longjmp(taskRegs[0],num);
		}
		dummy = setjmp(taskRegs[currTask]); //Save current task's regs in taskRegs[currTask]                 -------------
		if(TA3CTL & BIT0){     // check if timer finished counting
			TA3CTL &= ~BIT0;     // reset timer flag
			TA3CTL = 0x0114;     // start TA3 timer from zero in UP mode with ACLK
			counter++;             // increment counter
		}
		if(counter >= 10){//wait 10 seconds (TA3)
			HoldGreenLED = rand()%26; //place random num under 26 in global variable
			TA3CCR0 = 32768;     // set timer to count for 1 second
			TA3CTL = 0x0114;     // start TA3 timer from zero in UP mode with ACLK
			somethingInteresting();//mess with P1.0
		}
	}
}

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

    int cancel_prompt = 0;

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