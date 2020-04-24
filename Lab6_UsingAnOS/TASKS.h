//************************************************************************
// TASKS.H
// Tasks header file with required Global variables
//
//  <DESCRIPTION>
//
// <NAME>
// <DATE>   <VERSION>
// <PURPOSE>
//
//************************************************************************

#ifndef TASKS_H_
#define TASKS_H_

//************************************************************************
// Function Prototypes
//************************************************************************
int InitTasks(void);
int Task0(void);
int Task1(void);

//************************************************************************
// Global Variables
//************************************************************************
unsigned short NumTasks;        // Max is 5 Tasks, 0, 1, 2, 3, 4
unsigned short Task0Stack[16];  // Stack for Task 0
unsigned short Task1Stack[16];  // Stack for Task 0
unsigned short Task2Stack[16];  // Stack for Task 0
unsigned short Task3Stack[16];  // Stack for Task 0
unsigned short Task4Stack[16];  // Stack for Task 0
unsigned short TaskPointers[5]; // Array for Task Stack Addresses

//************************************************************************
#endif /* TASKS_H_ */
