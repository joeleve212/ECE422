/*
 * OB_JL_4bitLCD.c
 *
 *  Created on: Feb 12, 2020
 *      Author: Joe Leveille
 */

#include <msp430.h>

#define SMCLK 0x0200          // Timer_A SMCLK source
#define ACLK 0x0100 // Timer ACLK source
#define UP 0x0010 // Timer Up mode
#define STOP 0x0030 // stop Timer Up mode
#define TACLR_ 0x0004 // Clear TAxR
#define SLOW 0x00C0 // Slow down the clock TAxR
#define ENABLE_PINS 0xFFFE // Required to use inputs and outputs
#define DB7 0x04
#define DB6 0x08
#define DB5 0x04
#define DB4 0x04
#define ENABLE 0x08
#define RS 0x10

void clearScreen(){
	//
}
void setCursor(int location){ //0 -> 31

}
void printToLCD(char word[32]){ //>16 chars or scroll

}
void setRow(int rowNum){ //0 - 2, 2 being both

}
void delay(int microSecs){ //stay below 65 ms
	//TODO: adjust microSecs to CCR value
	//setup timer
	//start timer
	//Stay in this function until timer stops
	TA0CCR0 = microSecs;  //set length of timer
	TA0CTL = SMCLK + UP + TACLR;
	int i;
	while(!TAIFG){ //TODO: see if for loop can be avoided??
		for(i=0;i<1000;i++);
	}
}
void extLCDinit(){
	/*
	 * 9.2 = DB7
	 * 4.3 = DB6
	 * 4.2 = DB5
	 * 3.2 = DB4
	 * 9.3 = ENable
	 * 1.4 = RS
	 *
	 */
	PM5CTL0 = 0xFFFE; // Turn on the Digital I/o
	P9DIR |= DB7 | ENABLE;		//Activate P9 outputs
	P4DIR |= DB6 | DB5;		//Activate P4 outputs
	P3DIR |= DB4;		//Activate P3 outputs
	P1DIR |= 0x01 | RS;		//Activate P1 outputs
	P9OUT &= ~ENABLE; //make sure enable is low
	int stdDelay = 45, enHigh = 10;

	delay(16); // wait 15ms
	P9OUT &= ~DB7; //send DB7 - DB4[0,0,1,1], RS = 0
	P4OUT &= ~DB6; //set DB6
	P4OUT |= DB5; //set DB5
	P3OUT |= DB4; //set DB4
	P1OUT &= ~RS;
	P9OUT |= ENABLE; //send data
	delay(500); //wait for .5 ms before turning off enable
	P9OUT &= ~ENABLE;

	delay(6000);//wait for 4.1ms - .5ms
	P9OUT |= ENABLE; //send data
	delay(50); //wait for 50 us before turning off enable
	P9OUT &= ~ENABLE;

	delay(120);
	P9OUT |= ENABLE; //send DB7 - DB4[0,0,1,1], RS = 0
	delay(50); //wait for 50 us before turning off enable
	P9OUT &= ~ENABLE;

	P3OUT &= ~DB4;
	delay(stdDelay);//wait 37us
	P9OUT |= ENABLE;//send DB7 - DB4[0,0,1,0], RS = 0
	delay(enHigh); //wait before turning off enable
	P9OUT &= ~ENABLE;

	delay(stdDelay-enHigh); //wait the rest of 37us
	P9OUT |= ENABLE;//send DB7 - DB4[0,0,1,0], RS = 0
	delay(enHigh); //wait before turning off enable
	P9OUT &= ~ENABLE;

	P9OUT |= DB7; //set DB7 hi
	P4OUT |= DB6; //set DB6 hi
	P3OUT |= DB4; //set DB4 hi
	delay(stdDelay-enHigh); //wait the rest of delay
	P9OUT |= ENABLE;//send DB7 - DB4[1,1,1,1] //2 lines, font 1?, don't care?,don't care?
	delay(enHigh); //wait before turning off enable
	P9OUT &= ~ENABLE;

	P9OUT &= ~DB7; //set DB7 lo
	P4OUT &= ~DB6; //set DB6 lo
	P4OUT &= ~DB5; //set DB5 lo
	P3OUT &= ~DB4; //set DB4 lo
	delay(stdDelay-enHigh); //wait the rest of delay
	P9OUT |= ENABLE;//send DB7 - DB4[0,0,0,0], RS = 0
	delay(enHigh); //wait before turning off enable
	P9OUT &= ~ENABLE;

	P9OUT |= DB7; //set DB7 hi
	delay(stdDelay-enHigh); //wait the rest of delay
	P9OUT |= ENABLE;//send DB7 - DB4[1,0,0,0], RS = 0
	delay(enHigh); //wait before turning off enable
	P9OUT &= ~ENABLE;

	P9OUT &= ~DB7; //set DB7 lo
	delay(stdDelay-enHigh); //wait the rest of delay
	P9OUT |= ENABLE;//send DB7 - DB4[0,0,0,0], RS = 0
	delay(enHigh); //wait before turning off enable
	P9OUT &= ~ENABLE;

	P3OUT |= DB4; //set DB4 hi
	delay(stdDelay-enHigh); //wait the rest of delay
	P9OUT |= ENABLE;//send DB7 - DB4[0,0,0,1], RS = 0
	delay(enHigh); //wait before turning off enable
	P9OUT &= ~ENABLE;

	P3OUT &= ~DB4; //set DB4 lo
	delay(stdDelay-enHigh); //wait the rest of delay
	P9OUT |= ENABLE;//send DB7 - DB4[0,0,0,0], RS = 0
	delay(enHigh); //wait before turning off enable
	P9OUT &= ~ENABLE;

	//wait 37us

	P4OUT |= DB6; //set DB6
	P4OUT |= DB5; //set DB5
	delay(stdDelay-enHigh); //wait the rest of delay
	P9OUT |= ENABLE;//send DB7 - DB4[0,1,1,0], RS = 0 //idk,idk,dec(0)/inc(1),allow shifting?
	delay(enHigh); //wait before turning off enable
	P9OUT &= ~ENABLE;

	delay(stdDelay);
}

