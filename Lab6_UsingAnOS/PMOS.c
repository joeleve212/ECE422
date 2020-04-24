//************************************************************************
// RTOS.C
// RTOS Functions and Definitions
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
 int Task0(void);

//************************************************************************
// Includes
//************************************************************************
#include <msp430fr6989.h>   // Register Definitions
#include <PMOS.h>           // PMOS Definitions
#include <TASKS.h>          // TASKS Definitions

 //************************************************************************
 // Function Prototypes
 //************************************************************************
 int RunPMOS(void);
 int SetupPMOS(void);
 void Timer0_ISR (void);

//************************************************************************
// Function Defintions
//************************************************************************

int RunPMOS(void){
     // Start the Scheduler Clock and Enable Maskable Interrupts
     TA0CTL = 0x0214;    // start TA0 from 0 using SMCLK in UP mode
     _BIS_SR(GIE);       // Enable Maskable Interrupts

     // initialize the Task Counter and Start the first Task
    RunningTask = 0;  // Task Counter - the id number of the currently running Task
    Task0();

    return 0;
}//end RunPMOS()

int SetupPMOS(void){
    WDTCTL = 0x5A80;    // stop watchdog timer

    // Set up Scheduler Interrupt on TA0 Timer
    TA0CCR0 = 1000;     // Scheduler Interrupt every 1 ms
    TA0CCTL0 = CCIE;    // enable TA0 interrupt

    return 0;
}//end SetupPMOS()


//************************************************************************
// Interrupt Service Routines
//************************************************************************

// Timer TA0 Interrupt is the PMOS Scheduler and Full-Context Switcher
// No other interrupts are supported in this PMOS (No interrupts in Tasks)
//
// The Scheduler currently just goes through the Tasks in order
//
// The full-context switcher currently only takes care of the CPU registers
// Either use Task dedicated peripherals or handle the peripheral sharing
// within the Tasks

#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer0_ISR (void){
    // Full-Context Switcher - Save Current Task States (CPU Registers)

    //__asm("  PUSH.W R15");  // push current task R15 onto system stack
            // New Compiler adds a PUSHM.W #1,R15 Instruction
            // so we do not need to PUSH the R15 value ourselves
            // Check that your oompiler does this.  If not then
            // un-comment the above instruction AND the last instruction in this ISR

    __asm("  MOV.W SP, &SysSP");  // save current system SP

    CurrTaskSP = TaskPointers[RunningTask];
    __asm("  MOV.W CurrTaskSP, SP");  // make system SP point to end of current task stack
    __asm("  PUSH.W R4");  // save current task R4 onto current task stack
    __asm("  PUSH.W R5");  // save current task R5 onto current task stack
    __asm("  PUSH.W R6");  // save current task R6 onto current task stack
    __asm("  PUSH.W R7");  // save current task R7 onto current task stack
    __asm("  PUSH.W R8");  // save current task R8 onto current task stack
    __asm("  PUSH.W R9");  // save current task R9 onto current task stack
    __asm("  PUSH.W R10");  // save current task R10 onto current task stack
    __asm("  PUSH.W R11");  // save current task R11 onto current task stack
    __asm("  PUSH.W R12");  // save current task R12 onto current task stack
    __asm("  PUSH.W R13");  // save current task R13 onto current task stack
    __asm("  PUSH.W R14");  // save current task R14 onto current task stack
    CurrTaskSP -= 22;       // adjust current task SP address

    __asm("  MOV.W SysSP, SP");
    __asm("  POP.W R15");  // put current task R15 into R15
    __asm("  POP.W R14");  // put current task SR into R14
    __asm("  POP.W R13");  // put current task PC into R13
    SysSP += 6;           // adjust system SP address

    __asm("  MOV.W CurrTaskSP, SP");
    __asm("  PUSH.W R15");  // save current task R15 onto current task stack
    __asm("  PUSH.W R14");  // save current task SR onto current task stack
    __asm("  PUSH.W R13");  // save current task PC onto current task stack
    CurrTaskSP -= 6;       // adjust current task SP address

    // Scheduler choose next Task
    if(RunningTask >= (NumTasks - 1)) RunningTask = 0;
    else RunningTask++;     // increment task number to next task

    // Full-Context Switcher - Restore Next Task States (CPU Registers)
    NextTaskSP = TaskPointers[RunningTask] - 28;  // address of top of next task stack
    __asm("  MOV.W NextTaskSP, SP");
    __asm("  POP.W R15");  // put next task PC into R15
    __asm("  POP.W R14");  // put next task SR into R14
    __asm("  POP.W R13");  // put next task R15 into R13
    NextTaskSP += 6;       // adjust next task SP address

    __asm("  MOV.W SysSP, SP");
    __asm("  PUSH.W R15");  // push next task PC onto system stack
    __asm("  PUSH.W R14");  // push next task SR onto system stack
    __asm("  PUSH.W R13");  // push next task R15 onto system stack
    SysSP -= 6;           // adjust system SP address

    __asm("  MOV.W NextTaskSP, SP");
    __asm("  POP.W R14");  // restore next current task R14 into R14
    __asm("  POP.W R13");  // restore next current task R13 into R13
    __asm("  POP.W R12");  // restore next current task R12 into R12
    __asm("  POP.W R11");  // restore next current task R11 into R11
    __asm("  POP.W R10");  // restore next current task R10 into R10
    __asm("  POP.W R9");  // restore next current task R9 into R9
    __asm("  POP.W R8");  // restore next current task R8 into R8
    __asm("  POP.W R7");  // restore next current task R7 into R7
    __asm("  POP.W R6");  // restore next current task R6 into R6
    __asm("  POP.W R5");  // restore next current task R5 into R5
    __asm("  POP.W R4");  // restore next current task R4 into R4

    __asm("  MOV.W SysSP, SP");
    //__asm("  POP.W R15");  // restore current task R15 into R15
                             // Compiler adds a POPM.W #1,R15 Instruction
                             // so we do not need to the POP R15 value ourselves

}//end TA0 ISR  (end the PMOS Scheduler and Full-Context Switcher and goto next Task)
