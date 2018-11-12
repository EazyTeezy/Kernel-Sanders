/*
 * ISRs.c
 *
 *  Created on: Oct 12, 2018
 *      Author: NickB
 */

/*
 * ISRoutines.c
 *
 *  Created on: Sep 11, 2018
 *      Author: NickB
 */


#include "ISRs.h"
#include "MSGpasser.h"
#include "kernel.h"

#include <string.h>

int UartOutIdle = TRUE; // memory location for Uart idle status

void UART0_Init(void)
{
    volatile int wait;

    /* Initialize UART0 */
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCUART_GPIOA;   // Enable Clock Gating for UART0
    SYSCTL_RCGCUART_R |= SYSCTL_RCGCGPIO_UART0;   // Enable Clock Gating for PORTA
    wait = 0; // give time for the clocks to activate

    UART0_CTL_R &= ~UART_CTL_UARTEN;        // Disable the UART
    wait = 0;   // wait required before accessing the UART config regs

    // Setup the BAUD rate
    UART0_IBRD_R = 8;   // IBRD = int(16,000,000 / (16 * 115,200)) = 8.680555555555556
    UART0_FBRD_R = 44;  // FBRD = int(.680555555555556 * 64 + 0.5) = 44.05555555555556

    UART0_LCRH_R = (UART_LCRH_WLEN_8);  // WLEN: 8, no parity, one stop bit, without FIFOs)

    GPIO_PORTA_AFSEL_R = 0x3;        // Enable Receive and Transmit on PA1-0
    GPIO_PORTA_PCTL_R = (0x01) | ((0x01) << 4);         // Enable UART RX/TX pins on PA1-0
    GPIO_PORTA_DEN_R = EN_DIG_PA0 | EN_DIG_PA1;        // Enable Digital I/O on PA1-0

    UART0_CTL_R = UART_CTL_UARTEN;        // Enable the UART
    wait = 0; // wait; give UART time to enable itself.
}

void InterruptEnable(unsigned long InterruptIndex)
{
/* Indicate to CPU which device is to interrupt */
if(InterruptIndex < 32)
    NVIC_EN0_R = 1 << InterruptIndex;       // Enable the interrupt in the EN0 Register
else
    NVIC_EN1_R = 1 << (InterruptIndex - 32);    // Enable the interrupt in the EN1 Register
}

void UART0_IntEnable(unsigned long flags)
{
    /* Set specified bits for interrupt */
    UART0_IM_R |= flags;
}

void UART0_IntHandler(void)
{
/*
 * Simplified UART ISR - handles receive and xmit interrupts
 * Application signalled when data received
 */
    if (UART0_MIS_R & UART_INT_RX)
    {
        /* RECV done - clear interrupt and make char available to application */
        UART0_ICR_R |= UART_INT_RX;

        Data = UART0_DR_R;

        data_ptr = &Data; // here we assign a ptr to our data value as our k_send function requires a char pointer as a parameter


        /* going to want to enqueue data into a FIFO queue */
        /* this will be done by calling a function like 'send_msg()'*/
        k_send(TO_MON, UART_SRC, data_ptr, ONE_CHAR);

    }

    if (UART0_MIS_R & UART_INT_TX)
    {
        /* XMIT done - clear interrupt */
        UART0_ICR_R |= UART_INT_TX;


        /*-----------------------------------------------------------------------------*/
        /* The following is an outline of the four arguments being received             */
        /* by this process from its queue. The four arguments are as follows            */
        /*                                                                              */
        /*      arg 1: the message queue from which to receive our message              */
        /*      arg 2: pointer to the location for the message source to be stored      */
        /*              (not needed in this scope)                                       */
        /*      arg 3: pointer to the location for the message to be stored             */
        /*      arg 4: the size of the message to be recieved                           */
        /*                                                                              */
        /*       note: the return value is TRUE if the msg was succesfully recvd        */
        /*        and FALSE if the msg was not succesfully recvd                        */
        /*------------------------------------------------------------------------------*/

        // this is the new way

        if(k_uart_recv(&dta, &src)){ // this checks to see if there's anything waiting for us in the to UART queue

            /* this is where we will use all the information about position to output the character to the correct place on the screen*/
            UART0_DR_R = dta;
        }
        else{
            UartOutIdle = TRUE;
        }

    }
}


void UART_sendChar(char CharToUart)
{
    UART0_DR_R = CharToUart;
    UartOutIdle = FALSE;
}

void InterruptMasterEnable(void)
{
    /* enable CPU interrupts */
    __asm(" cpsie   i");
}


/* SYSTICK */

void SysTickStart(void)
{
    // Set the clock source to internal and enable the counter to interrupt
    ST_CTRL_R |= ST_CTRL_CLK_SRC | ST_CTRL_ENABLE;
}

void SysTickStop(void)
{
    // Clear the enable bit to stop the counter
    ST_CTRL_R &= ~(ST_CTRL_ENABLE);
}

void SysTickPeriod(unsigned long Period)
{
    /*
     For an interrupt, must be between 2 and 16777216 (0x100.0000 or 2^24)
     */
    ST_RELOAD_R = Period - 1;  /* 1 to 0xff.ffff */
}

void SysTickIntEnable(void)
{
    // Set the interrupt bit in STCTRL
    ST_CTRL_R |= ST_CTRL_INTEN;
}

void SysTickIntDisable(void)
{
    // Clear the interrupt bit in STCTRL
    ST_CTRL_R &= ~(ST_CTRL_INTEN);
}


void SysTickHandler(void)
{

   // unsigned long *nextStkPtr; // points to the top of our new process stack
   //struct stack_frame *nextStk; // points to the top of our register and special reg stack frame in hi memory in our stack


    // first we need to save the register
    save_registers();

    //next we will put our currently running process in the appchange our stack pointer
    running[high_priority]->sp = (struct stack_frame*)get_PSP();

    //now moving the running pointer to the next PCB
    running[high_priority] = running[high_priority]->next;




    // we will set our new psp
    set_PSP((unsigned long)running[high_priority]->sp);

    //finally lets restore our registers
    restore_registers();

}


