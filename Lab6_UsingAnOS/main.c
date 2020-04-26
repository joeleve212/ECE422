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
char priority[3] = {0, 1, 1};     //set the priority of each task (only task 3 is priority)
char pCard[2] ={'K', '4'},pSuit[2]={'C','D'}; //place dummy cards initially (never displayed)
int drawFlag = 0, printNow = 0;

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

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer
    PM5CTL0 = ENABLE_PINS; 				// Enable inputs and outputs
    HoldGreenLED = 3; // place some default value for the LED
	initGPIO();                // Initializes Inputs and Outputs for LCD
	initClocks();              // Initialize clocks for LCD
	myLCD_init();              // Prepares LCD to receive commands
    //use ScrollWords to show directions
    //order of suits: P, H, D, C
    ScrollWords("WELCOME TO CARD BATTLES    TWO SIMULATED PLAYERS USED    EACH DRAWS CARD FROM 52 CARD DECK    P1 ON LEFT 2 CHARS AND P2 ON RIGHT 2 CHARS");
    ScrollWords("WINNER IS DISPLAYED IN CENTER");
    ScrollWords("ORDER OF DOMINANT SUITS FROM HIGHEST        SPADE  HEART  DIAMOND  CLUB");
    ScrollWords("P USED FOR SPADES     T USED FOR TENS      FIRST LETTER USED FOR FACES AND OTHER SUITS");

    TA0CCR0 = 10000;     // setup TA0 timer to count for default length of time
    TA0CTL = 0x0114;     // start TA0 timer from zero in UP mode with ACLK
    TA0CCTL0 = CCIE; // Enable interrupt for Timer0
    _BIS_SR(GIE); // Enter activate interrupts
	return 0;
}
#pragma vector=TIMER0_A0_VECTOR
__interrupt void OStimer(void){ //handle task incrementing
	TA0CCTL0 = TA0CCTL0 & ~TAIFG;    //reset ISR flag
	static const int defaultTime = 1500, longTime = 2500; //set time lengths

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
	unsigned char k;
	unsigned char debounce = 0;
	unsigned int i, buttonUnpress;
	while(1){
		debounce = 0;
		for (k=0;k<15;k++){// check buttons 15 times
			for(i=0;i<100;i++);// delay to check buttons  (~1ms)
			if(!(P1IN & 0x02)) debounce++;   // if pressed count
		}
		if (debounce > 5 & buttonUnpress == 1){    // pressed 5 of 15 times so a button is pressed
			drawFlag = 1; //Set draw flag = 1,
			buttonUnpress = 0;
			TA0CCTL0 = TA0CCTL0 | TAIFG; //Throw timer flag
		} else if(debounce <= 5){
			buttonUnpress = 1; // make sure a button press only triggers one roll
		}
	}
}

void interpPull(int pull, int playerNum){ //populate suit and card array vars
	switch(pull/13){ //assign suits
		case 0:
			pSuit[playerNum-1]='H';
			break;
		case 1:
			pSuit[playerNum-1]='C';
			break;
		case 2:
			pSuit[playerNum-1]='D';
			break;
		default:
			pSuit[playerNum-1]='P';
			break;
	}
	switch(pull%13){
		case 12:
			pCard[playerNum-1]='A';
			break;
		case 11:
			pCard[playerNum-1]='K';
			break;
		case 10:
			pCard[playerNum-1]='Q';
			break;
		case 9:
			pCard[playerNum-1]='J';
			break;
		case 8:
			pCard[playerNum-1]='T';
			break;
		default:
			pCard[playerNum-1]=(pull%13)+50;		//convert int to char value for numeral cards < 10
			break;
	}
}

void drawCards(){
	int p1Pull = rand() % 52; //randomly pick 0-51 (since 52 card deck)
	int p2Pull = rand() % 52; //randomly pick 0-51 (since 52 card deck)
	while(p1Pull==p2Pull){	  //pick new card for p2 until it's different from p1
		p2Pull = rand() % 52;
	}
	interpPull(p1Pull,1);
	interpPull(p2Pull,2);
}

void task2(){
	//TODO: setup vars & randomization
	srand(time(NULL));
	while(1){
		if(drawFlag==1){ //check if draw is needed
			//TODO: trigger drawing suit and card, prevent double
			drawCards();
			printNow = 1; //set ready to print flag
			drawFlag = 0;
			TA0CCTL0 = TA0CCTL0 | TAIFG; //Throw timer flag
		} else{
			//TODO: if not, return? or go to next? or keep checking?
			printNow = 0; //clear ready to print flag
			TA0CCTL0 = TA0CCTL0 | TAIFG; //Throw timer flag
		}
	}
}

int cardVal(char card, char pNum){
	int val;
	if(pCard[pNum-1]>'9'){ //If represented by a letter
			switch(card){
				case 'T':
					val = 10;
					break;
				case 'J':
					val = 11;
					break;
				case 'Q':
					val = 12;
					break;
				case 'K':
					val = 13;
					break;
				case 'A':
					val = 14;
					break;
			}
		} else{
			val = card - 48;  //converts from ASCII -> int, so '2' (int 50) -> int 2
		}
	return val;
}

char findWinner(){ //calculate who won - order of suits: S, H, D, C & Ace is High & 10 shown as 'T'
	int val1,val2;

	val1 = cardVal(pCard[0],1);
	val2 = cardVal(pCard[1],2);

	if(val1>val2){ //player 1 wins
		return '1';
	} else if(val2>val1){ //player 2 wins
		return '2';
	} else{ //same card, different suit
		if(pSuit[0]>pSuit[1]){ //player 1 wins
			return '1';
		} else{ //player 2 wins
			return '2';
		}
	}
}

void task3(){
	while(1){
		if(printNow == 1){ //if ready to print
			//print cards
			myLCD_showChar(pCard[0],1);
			myLCD_showChar(pSuit[0],2);
			myLCD_showChar(pCard[1],5);
			myLCD_showChar(pSuit[1],6);

			//determine winner & print result in spots 3 & 4
			myLCD_showChar('P',3);
			char winner = findWinner();
			myLCD_showChar(winner,4);

			printNow = 0; //clear flag
		} else {
			TA0CCTL0 = TA0CCTL0 | TAIFG; //Throw timer flag if not printing
		}
	}

}

