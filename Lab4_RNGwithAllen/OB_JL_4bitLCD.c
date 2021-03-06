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
#define TACLR 0x0004 // Clear TAxR
#define SLOW 0x00C0 // Slow down the clock TAxR
#define ENABLE_PINS 0xFFFE // Required to use inputs and outputs
#define DB7 0x04
#define DB6 0x08
#define DB5 0x04
#define DB4 0x04
#define ENABLE 0x08
#define RS 0x10
#define stdDelay 2

char curPosition; // variable that tracks cursor position

void delay(unsigned int milliSecs){
	TA1CCR0 = 1000;
	TA1CTL = 0x0214;
	while(1){
		if(TA1CTL&BIT0){
			TA1CTL &= ~BIT0;
			milliSecs--;
			if(milliSecs==0)break;
		}
	}
	TA1CTL = 0x0204; //stop timer
}

void sendMessage(){
	P9OUT |= ENABLE; //send data
	int enHigh = 1;
	delay(enHigh); //wait 10 us before turning off enable
	P9OUT &= ~ENABLE;
}
void clearScreen(){
	delay(2);
	P9OUT &= ~DB7; //set DB7 lo
	P4OUT &= ~DB6; //set DB6 lo
	P4OUT &= ~DB5; //set DB5 lo
	P3OUT &= ~DB4; //set DB4 lo
	sendMessage();//send DB7 - DB4[0,0,0,0], RS = 0

	P3OUT |= DB4; //set DB4 hi
	delay(stdDelay); //wait the rest of delay
	sendMessage();//send DB7 - DB4[0,0,0,1], RS = 0
	curPosition = 0;
	delay(2);
}
void setCursor(char location){ //0 -> 31
	//PLACE CURSOR (theoretically)
	delay(3);
	P9OUT &= ~ENABLE;
	P1OUT &= ~RS;
	P9OUT |= DB7; //set DB7 hi
	if(location>15){
		P4OUT |= DB6; //set DB6 hi
	} else{
		P4OUT &= ~DB6; //set DB6 lo
	}
	P4OUT &= ~DB5; //set DB5 lo
	P3OUT &= ~DB4; //set DB4 lo
	sendMessage();				//Send first half of char
	delay(stdDelay);

	if(location & 0x08){
		P9OUT |= DB7; //set DB7 hi
	} else{
		P9OUT &= ~DB7; //set DB7 lo
	}
	if(location & 0x04){
		P4OUT |= DB6; //set DB6 hi
	} else{
		P4OUT &= ~DB6; //set DB6 lo
	}
	if(location & 0x02){
		P4OUT |= DB5; //set DB5 hi
	} else{
		P4OUT &= ~DB5; //set DB5 lo
	}
	if(location & 0x01){
		P3OUT |= DB4; //set DB4 hi
	} else{
		P3OUT &= ~DB4; //set DB4 lo
	}
	sendMessage();
	delay(stdDelay);
	curPosition = location; // update position tracker
	delay(3);
}
void printToLCD(char* word){ //>16 chars or scroll
	for(;*word!=0;word++){ //
		P1OUT |= RS;
		if(*word & 0x80){
			P9OUT |= DB7; //set DB7 hi
		} else{
			P9OUT &= ~DB7; //set DB7 lo
		}
		if(*word & 0x40){
			P4OUT |= DB6; //set DB6 hi
		} else{
			P4OUT &= ~DB6; //set DB6 lo
		}
		if(*word & 0x20){
			P4OUT |= DB5; //set DB5 hi
		} else{
			P4OUT &= ~DB5; //set DB5 lo
		}
		if(*word & 0x10){
			P3OUT |= DB4; //set DB4 hi
		} else{
			P3OUT &= ~DB4; //set DB4 lo
		}
		sendMessage();
		delay(3); //finish delay
		if(*word & 0x08){
			P9OUT |= DB7; //set DB7 hi
		} else{
			P9OUT &= ~DB7; //set DB7 lo
		}
		if(*word & 0x04){
			P4OUT |= DB6; //set DB6 hi
		} else{
			P4OUT &= ~DB6; //set DB6 lo
		}
		if(*word & 0x02){
			P4OUT |= DB5; //set DB5 hi
		} else{
			P4OUT &= ~DB5; //set DB5 lo
		}
		if(*word & 0x01){
			P3OUT |= DB4; //set DB4 hi
		} else{
			P3OUT &= ~DB4; //set DB4 lo
		}
		sendMessage();
		delay(30); //finish delay
		curPosition++;
		if(curPosition == 16){
			P1OUT &= ~RS;
			setCursor(16);
		}
		else if(curPosition > 31){
			P1OUT &= ~RS;
			clearScreen();
			printToLCD("Error: input too long");
			delay(4000);
			clearScreen();
			return;
		}
	}
	P1OUT &= ~RS;
}
void setRow(int rowNum){ //0 - 2, 2 being both

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

	delay(16); // wait 15us
	P9OUT &= ~DB7; //send DB7 - DB4[0,0,1,1], RS = 0
	P4OUT &= ~DB6; //set DB6
	P4OUT |= DB5; //set DB5
	P3OUT |= DB4; //set DB4
	P1OUT &= ~RS;
	sendMessage(); //send data

	delay(5);//wait for more than 4.1ms
	sendMessage();//send data

	delay(2);
	sendMessage(); //send DB7 - DB4[0,0,1,1], RS = 0
	delay(5); //wait for 50 us before next message

	P3OUT &= ~DB4;
	sendMessage();//send DB7 - DB4[0,0,1,0], RS = 0


	delay(stdDelay); //wait the rest of 37us
	sendMessage();//send DB7 - DB4[0,0,1,0], RS = 0

	P9OUT |= DB7; //set DB7 hi
	P4OUT |= DB6; //set DB6 lo
	P3OUT |= DB4; //set DB4 hi
	delay(stdDelay); //wait the rest of delay
	sendMessage();//send DB7 - DB4[1,1,1,1] //2 lines, font 1?, don't care?,don't care?

	P9OUT &= ~DB7; //set DB7 lo
	P4OUT &= ~DB6; //set DB6 lo
	P4OUT &= ~DB5; //set DB5 lo
	P3OUT &= ~DB4; //set DB4 lo
	delay(stdDelay); //wait the rest of delay
	sendMessage();//send DB7 - DB4[0,0,0,0], RS = 0

	P3OUT |= DB4; //set DB4 hi
	delay(stdDelay); //wait the rest of delay
	sendMessage();//send DB7 - DB4[0,0,0,1], RS = 0

	P3OUT &= ~DB4; //set DB4 lo
	delay(stdDelay); //wait the rest of delay
	sendMessage();//send DB7 - DB4[0,0,0,0], RS = 0

	//wait 37us

	P4OUT |= DB6; //set DB6
	P4OUT |= DB5; //set DB5
	delay(stdDelay); //wait the rest of delay
	sendMessage();//send DB7 - DB4[0,1,1,0], RS = 0 //idk,idk,dec(0)/inc(1),allow shifting?

	delay(stdDelay);
	//TURN ON CURSOR (theoretically)
	P9OUT &= ~DB7; //set DB7 lo
	P4OUT &= ~DB6; //set DB6 lo
	P4OUT &= ~DB5; //set DB5 lo
	P3OUT &= ~DB4; //set DB4 lo
	sendMessage();
	delay(stdDelay);
	P9OUT |=DB7; //set DB7 hi
	P4OUT |= DB6; //set DB6 hi
	P4OUT |= DB5; //set DB5 hi
	P3OUT |= DB4; //set DB4 hi
	sendMessage();
	delay(stdDelay);
}

