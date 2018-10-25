
/**
 * main.c
 */

#define MAX_ENTRIES 4

#include <stdio.h>
#include <string.h>

#include "ISRs.h"
#include "MSGpasser.h"
#include "menu.h"
#include "kernel.h"

// global variable to count number of interrupts on PORTF0 (falling edge)
volatile int count = 0;


int main(void)
{

    /* Initialize UART */
    UART0_Init();           // Initialize UART0
    InterruptEnable(INT_VEC_UART0);       // Enable UART0 interrupts
    UART0_IntEnable(UART_INT_RX | UART_INT_TX); // Enable Receive and Transmit interrupts


    InterruptMasterEnable();      // Enable Master (CPU) Interrupts


  //  user_menu(); // this is where the user can enter their processes in a menu. reg_proc() is contained in this function

   // reg_proc("P1", PRIORITY1, 1); // the x is just to fill in the priority section for now

    reg_proc("P2", PRIORITY1, 2);

    reg_proc("P3", PRIORITY1, 3);
 //reg_proc("P2", PRIORITY1, 2);
 reg_proc("P1", PRIORITY1, 1);
// reg_proc("P2", PRIORITY1, 2);
 //   reg_proc("P3", PRIORITY1, 3);
    reg_proc("IDL", PRIORITY1, 4);

    start(); // this will start the processes and run them. the kernel is also where other stuff will happen


    return 0;
}
