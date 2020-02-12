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
void delay(int microSecs);
void extLCDinit();

