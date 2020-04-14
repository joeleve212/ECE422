/*
* ECE 422 - RNG Project
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

int roll(){ //TODO: roll the dice and return the total roll
	int die1 = (rand() % sides)+1, die2 = (rand() % sides)+1, total = die1 + die2;
	return total;
}

int main(void) {
	ADC_SETUP(); 			   // Sets up ADC peripheral
	WDTCTL = 0x5A80;           // Disable WDT (poor doggy!)
	initGPIO();                // Initializes Inputs and Outputs for LCD
	initClocks();              // Initialize clocks for LCD
	myLCD_init();              // Prepares LCD to receive commands






	myLCD_displayNumber((ADC12MEM0*330000)/4096);     // Display the number

	return 0;
}

#pragma vector=PORT1_VECTOR
__interrupt void buttonPressed(void){
	ADC12CTL0 = ADC12CTL0 & (~ADC12ENC); // Need to disable peripheral
	srand((int)(ADC12MEM0 & 0x011));     // seed randomization with bottom 8 bits of ADC input
	if(P1IFG & 0x02){ 				//P1.1
		//TODO: activate roll
		roll();
	} else if(P1IFG & 0x04){ 		//P1.2
		//TODO: detect short or long press, then increment numDice or numSides
	}
	P1IFG ^= P1IFG; 				//Clear button flags
	if(P1IV);
	ADC12CTL0 = ADC12CTL0 | ADC12ENC; // Enable conversion
	ADC12CTL0 = ADC12CTL0 | ADC12SC; // Start next conversion
}
