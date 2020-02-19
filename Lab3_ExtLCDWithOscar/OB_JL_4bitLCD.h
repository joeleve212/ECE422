/*
 * OB_JL_4bitLCD.h
 *
 *  Created on: Feb 12, 2020
 *      Author: Joe Leveille
 */

#ifndef OB_JL_4BITLCD_H_
#define OB_JL_4BITLCD_H_

void extLCDinit();
void clearScreen();
void setCursor(int location); //0 -> 31
void printToLCD(char* word); //>16 chars or scroll
void setRow(int rowNum); //0 - 2, 2 being both
void delay(int microSecs);


#endif /* OB_JL_4BITLCD_H_ */
