//************************************************************************
// MAIN.C
// The main function to demo the PMOS ith two simple test Tasks
//
//  <DESCRIPTION>
//
// <NAME>
// <DATE>   <VERSION>
// <PURPOSE>
//
//************************************************************************
// Requires the following External Functions
//************************************************************************
int SetupPMOS(void);
int InitTasks(void);
int RunPMOS(void);
//
//************************************************************************
// Includes
//************************************************************************
#include <msp430fr6989.h>   // Register Definitions
#define ENABLE_PINS 0xFFFE // Required to use inputs and outputs
//************************************************************************
// Function Prototypes
//************************************************************************
int main(void);

//************************************************************************
// Main Function
//************************************************************************
int main(void){
    WDTCTL = 0x5A80;    // stop watchdog timer

    SetupPMOS();
    InitTasks();
    RunPMOS();

    // Error if program reaches this point - RESET
    WDTCTL = 0x0000;

    return 0;
}//end main()
