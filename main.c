
/**
 * main.c
 */

#define MAX_ENTRIES 13

#include <stdio.h>
#include <string.h>

#include "ISRs.h"
#include "MSGpasser.h"
#include "menu.h"
#include "kernel.h"


/* Software Functionality Use Cases. Uncomment demonstration to run */

//#define PRIORITY_NICE_DEMO        1
//#define MULTI_PROC_DEMO           2
//#define MSG_PASS_DEMO             3
//#define IO_DEMO                   4
//#define IDLE_PROC_DEMO            5
//#define BLOCKED_PROC_DEMO         6
//#define CUP_DEMO                  7



// global variable to count number of interrupts on PORTF0 (falling edge)
volatile int count = 0;


int main(void)
{

    /* Initialize UART */
    UART0_Init();           // Initialize UART0
    InterruptEnable(INT_VEC_UART0);       // Enable UART0 interrupts
    UART0_IntEnable(UART_INT_RX | UART_INT_TX); // Enable Receive and Transmit interrupts

    SysTickPeriod(MAX_WAIT);
    SysTickIntEnable();
    SysTickStart();


    InterruptMasterEnable();      // Enable Master (CPU) Interrupts


  //  user_menu(); // this is where the user can enter their processes in a menu. reg_proc() is contained in this function

     /* PRIORITY_NICE_DEMO */
    #ifdef PRIORITY_NICE_DEMO

        reg_proc("P3", PRIORITY3, 3); //
        reg_proc("P6", PRIORITY2, 6); //
        reg_proc("P7", PRIORITY2, 7); //
        reg_proc("P10", PRIORITY4, 10);

        /* an ide process which prints out idle ... forever*/
        reg_proc("IDL", IDLE_PRIORITY, 4);

    #endif

    /* MULTI_PROC_DEMO */
    #ifdef MULTI_PROC_DEMO

        reg_proc("P6", PRIORITY1, 6); //
        reg_proc("P7", PRIORITY1, 7); //
        reg_proc("P8", PRIORITY1, 8); //

        /* an ide process which prints out idle ... forever*/
        reg_proc("IDL", IDLE_PRIORITY, 4);

    #endif
    /* MSG_PASS_DEMO */
    #ifdef MSG_PASS_DEMO

        reg_proc("P2", PRIORITY1, 2); // this is acting as a receiveing processes
        reg_proc("P1", PRIORITY1, 1); // this processes is sending
        reg_proc("P4", PRIORITY1, 4); // this processes is sending

        /* an ide process which prints out idle ... forever*/
        reg_proc("IDL", IDLE_PRIORITY, 4);

    #endif

       /* IO_DEMO */
    #ifdef IO_DEMO

        reg_proc("MN", PRIORITY1, 14); // this is acting as a receiveing processes ( 9 in the process id which matches the TO_UART index)


        /* an ide process which prints out idle ... forever*/
        reg_proc("IDL", IDLE_PRIORITY, 4);

    #endif

       /* IDLE_PROC_DEMO */
    #ifdef IDLE_PROC_DEMO

        reg_proc("P9", PRIORITY1, 9); // this is acting as a receiveing processes
        /* an ide process which prints out idle ... forever*/
        reg_proc("IDL", IDLE_PRIORITY, 4);

    #endif

        /* BLOCKED_PROC_DEMO */
    #ifdef BLOCKED_PROC_DEMO

    #endif

        /* CUP_DEMO */
    #ifdef CUP_DEMO

        reg_proc("P5", PRIORITY1, 5); // this is acting as a receiveing processes ( 9 in the process id which matches the TO_UART index)


        /* an ide process which prints out idle ... forever*/
        reg_proc("IDL", IDLE_PRIORITY, 4);

    #endif



    start(); // this will start the processes and run them. the kernel is also where other stuff will happen


    return 0;
}
