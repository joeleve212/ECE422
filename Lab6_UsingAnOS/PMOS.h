//************************************************************************
// PMOS.H
// PMOS header file with required Global variables
//
//  <DESCRIPTION>
//
// <NAME>
// <DATE>   <VERSION>
// <PURPOSE>
//
//************************************************************************

#ifndef PMOS_H_
#define PMOS_H_

//************************************************************************
// Function Prototypes
//************************************************************************
int SetupPMOS(void);
void Timer0_ISR (void);

//************************************************************************
// Global Variables
//************************************************************************
unsigned short RunningTask = 0; // task number of the currently running task
unsigned short SysSP;         // Place to store the address of the system stack
unsigned short CurrTaskSP;
unsigned short NextTaskSP;

//************************************************************************
#endif /* PMOS_H_ */
