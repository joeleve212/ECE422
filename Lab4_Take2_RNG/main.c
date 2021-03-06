/*
* ECE 422 - RNG Project with Reduced Scope
* Joe Leveille
* 4/20/20
*/
#include <msp430.h> 
#include <stdlib.h>
#include <time.h>
#include <driverlib.h> // Required for the LCD
#include "myGpio.h" // Required for the LCD
#include "myClocks.h" // Required for the LCD
#include "myLcd.h" // Required for the LCD
#define ENABLE_PINS 0xFFFE // Required to use inputs and outputs
int numDice = 1, numSides = 6; //set default die set up

void ScrollWords(char words[250]){
	unsigned int length; // Contains length of message to be displayed
	unsigned int slot; // Slot to be displayed on LCD (1, 2, 3, 4, 5, or 6)
	unsigned int amount_shifted; // Number of times message shifted so far
	unsigned int offset; // Used with amount_shifted to get correct character to display
	unsigned long delay; // Used to implement delay between scrolling iterations
	unsigned char next_char; // Next character from message to be displayed
	length = strlen(words); // Get the length of the message stored in words
	amount_shifted=0; // We have not shifted the message yet
	offset=0; // There is no offset yet
	while( amount_shifted < length+7 ){ // Loop as long as you haven't shifted all
					    // of the characters off the LCD screen
		offset=amount_shifted; // Starting point in message for next LCD update
		for(slot = 1;slot<=6;slot++){ // Loop 6 times to display 6 characters at a time
			next_char = words[offset-6]; // Get the current character for LCD slot
			if(next_char && (offset>=6) && (offset<=length+6) ){ // If character is not null AND
				// LCD is not filled (offset>=6) AND
				// You have not reached end of message
				// (offset<=length+6)
				myLCD_showChar(next_char,slot); // Show the next character on the LCD
				// screen in correct slot
			}
			else{ // Else, slot on LCD should be blank
				myLCD_showChar(' ',slot); // So, add a blank space to slot
			}
			offset++; // Update as you move across the message
		}// end for
		for(delay=0 ; delay<150000 ; delay++); // Delay between shifts
		amount_shifted = amount_shifted + 1; // Update times words shifted across LCD
	}// end while
}// end ScrollWords

void ADC_SETUP(void){
	#define ADC12_SHT_16 0x0200 // 16 clock cycles for sample and hold
	#define ADC12_ON 0x0010 // Used to turn ADC12 peripheral on
	#define ADC12_SHT_SRC_SEL 0x0200 // Selects source for sample & hold
	#define ADC12_12BIT 0x0020 // Selects 12-bits of resolution
	#define ADC12_P84 0x0007 // Use input P8.4 for analog input
	#define ADC12_P85 0x0006 // Use input P8.5 for analog input
	#define ADC12_P86 0x0005 // Use input P8.6 for analog input
	#define ADC12_P87 0x0004 // Use input P8.7 for analog input
	#define ADC12_P90 0x0008 // Use input P9.0 for analog input
	#define ADC12_P91 0x0009 // Use input P9.1 for analog input
	#define ADC12_P92 0x000A // Use input P9.2 for analog input
	#define ADC12_P93 0x000B // Use input P9.3 for analog input
	#define ADC12_P95 0x000D // Use input P9.5 for analog input
	#define ADC12_P96 0x000E // Use input P9.6 for analog input
	ADC12CTL0 = ADC12_SHT_16 | ADC12_ON ; // Turn on, set sample & hold time
	ADC12CTL1 = ADC12_SHT_SRC_SEL; // Specify sample & hold clock source
	ADC12CTL2 = ADC12_12BIT; // 12-bit conversion results
	ADC12MCTL0 = ADC12_P92; // P9.2 is analog input
}

int roll(){ // roll the dice and return the total roll
	int i, total = 0;			//start total roll at 0
	for(i=0;i<numDice;i++){			//loops for the each die being rolled
		total += (rand() % numSides)+1; //adds random value from 1 - numSides
	}
	return total;
}

int main(void) {
	ADC_SETUP(); 		// Sets up ADC peripheral
	WDTCTL = 0x5A80;    // Disable WDT (poor doggy!)
	initGPIO();         // Initializes Inputs and Outputs for LCD
	initClocks();       // Initialize clocks for LCD
	myLCD_init();       // Prepares LCD to receive commands

	P1DIR = P1DIR & ~BIT1; 			// P1.1 (Button S1) will be an input
	P1REN = P1REN | BIT1;			// P1.1 will have a pull-up
	P1OUT = P1OUT | BIT1; 			// resistor.

	P1DIR = P1DIR & ~BIT2; 			// P1.2 (Button S1) will be an input
	P1REN = P1REN | BIT2;			// P1.2 will have a pull-up
	P1OUT = P1OUT | BIT2; 			// resistor.
	P1IE |= BIT1; 		// Enable interrupt ONLY for P1.1
	P1IES |= BIT1; 		// for transitions from HI-->LO
	P1IE |= BIT2; 		// Enable interrupt ONLY for P1.2
	P1IES |= BIT2; 		// for transitions from HI-->LO
	_BIS_SR(GIE); 		//activate interrupts
	
	//Scroll instructions on screen
//	ScrollWords("USE L BUTTON TO ROLL DICE   USE R BUTTON TO INCREMENT NUMBER OF SIDES AND DICE USED");
//	ScrollWords("FIRST DIGIT IS NUMBER OF DICE ROLLING    NEXT 2 DIGITS ARE SIDES PER DIE   LAST 2 DIGITS ARE SUM OF YOUR ROLL");

	return 0;
}

#pragma vector=PORT1_VECTOR
__interrupt void buttonPressed(void){
	unsigned char k;
	int rollTot = 0;
	unsigned char debounce = 0, duration = 0;
	unsigned int i;
	for (k=0;k<15;k++){ // check buttons 15 times - Normal Debounce
		for(i=0;i<100;i++); // delay to check buttons (~1ms)
		if(!(P1IN & 0x02) | !(P1IN & 0x04)) debounce++; // if pressed count
	}

	if (debounce > 5){ // pressed 5 of 15 times so a button is pressed (P1.1 or P 1.2)
		ADC12CTL0 = ADC12CTL0 & (~ADC12ENC); // Need to disable ADC peripheral while taking measurement
		srand((int)(ADC12MEM0 & 0x0FF));     // seed randomization with bottom 8 bits of ADC input
		if(P1IFG & 0x02){ 				//P1.1 is rolling button
			rollTot = roll();
		} else if(P1IFG & 0x04){ 		//P1.2 adjusts numSides and numDice
			numSides = numSides % 20; 	//loops after max of 20 sides (range 6 - 20)
			if(numSides==0){
				numDice = (numDice % 3)+1; //loop after max of 4 dice (range 1 - 3)
				numSides = 6; 		   //Set back to minimum numSides
			}else{
				numSides++;		   //increment number of sides per die
			}

		}
		int finalDispNum = ((numSides+(100*numDice))*100)+rollTot;  //calculate single value to show: numDice, numSides, rollTot in that order
		myLCD_displayNumber(finalDispNum);			     //Display calculated number to the LCD
		ADC12CTL0 = ADC12CTL0 | ADC12ENC; 			     // Enable conversion
		ADC12CTL0 = ADC12CTL0 | ADC12SC; 			     // Start next conversion for seed randomization
	}
	P1IFG ^= P1IFG; 	//Clear button flags
	if(P1IV);		//Clear Interrupt vector
}
