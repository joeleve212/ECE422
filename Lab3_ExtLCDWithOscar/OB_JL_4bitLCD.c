/*
 * OB_JL_4bitLCD.c
 *
 *  Created on: Feb 12, 2020
 *      Author: Joe Leveille
 */

#include <msp430.h>

void clearScreen();
void setCursor(int location); //0 -> 31
void printToLCD(char word[32]); //>16 chars or scroll
void setRow(int rowNum); //0 - 2, 2 being both
void delay(int microSecs){
	//TODO: adjust microSecs to CCR value
	//setup timer
	//start timer
	//Stay in this function until timer stops
}
void extLCDinit(){
	//TODO: wait 15ms
	//send RS = 0, DB7 - DB4[0,0,1,1]
	//wait 4.1ms
	//send RS = 0, DB7 - DB4[0,0,1,1]
	//wait 100us
	//send RS = 0, DB7 - DB4[0,0,1,1]
	//wait 37us
	//send RS = 0, DB7 - DB4[0,0,1,0]
	//wait 37us
	//send RS = 0, DB7 - DB4[0,0,1,0]
	//wait 37us
	//send RS = 0, DB7 - DB4[1,1,1,1] //2 lines, font 1?, don't care?,don't care?
	//wait 37us
	//send RS = 0, DB7 - DB4[0,0,0,0]
	//wait 37us
	//send RS = 0, DB7 - DB4[1,0,0,0]
	//wait 37us
	//send RS = 0, DB7 - DB4[0,0,0,0]
	//wait 37us
	//send RS = 0, DB7 - DB4[0,0,0,1]
	//wait 37us
	//send RS = 0, DB7 - DB4[0,0,0,0]
	//wait 37us
	//send RS = 0, DB7 - DB4[0,1,0,0] //idk,idk,dec(0)/inc(1),allow shifting?
	//wait 37us

}

