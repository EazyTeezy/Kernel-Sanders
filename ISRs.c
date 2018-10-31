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

        /* going to want to enqueue data into a FIFO queue */
        /* this will be done by calling a function like 'send_msg()'*/
        send_msg(Data, UARTsrc, MONq); //UARTsrc defined in ISRoutines.h, other option is CLOCKsrc
    }

    if (UART0_MIS_R & UART_INT_TX)
    {

        /* XMIT done - clear interrupt */
        UART0_ICR_R |= UART_INT_TX;

        if(rec_msg(&dta, &sourc, UARTq)){ // this checks to see if there's anything waiting for us in the to UART queue
            UART0_DR_R = dta;
        }
        else{
            UartOutIdle = TRUE;
        }
    }
}

void InterruptMasterEnable(void)
{
    /* enable CPU interrupts */
    __asm(" cpsie   i");
}

void UART_sendChar(char CharToUart)
{
    UART0_DR_R = CharToUart;
    UartOutIdle = FALSE;
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


