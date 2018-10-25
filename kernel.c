/*
 * kernel.c
 * * Support kernel functions for process management
 * Remember:
 * - R0 is ARG1
 * - R0 has function return value
 *  Created on: Oct 12, 2018
 *      Author: NickB
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


#include "procs.h"
#include "kernel.h"
#include "MSGpasser.h"


/* here is where the 3 different process table entries are built */
struct fentry flist[MAX_ENTRIES] = {{proc1, "P1", 8}, {proc2, "P2", 8}, {proc3, "P3", 8}, {idle_proc, "IDL", 8}};

struct pcb *running = NULL;
struct pcb *ll_head = NULL;

int llCount = 0; // the number of items in our linked list (used for debugging purposes)

void reg_proc(char * prc, int pri, int pID){ // this is our argument)

    int j = 0; // used when searching unique processes table for matching name
    int i = 0; // used when outputting the matched process name
    char displayStr[14] = "added to list"; // sending in a '\n' here messes everything up with p1

    int prcMatch = FALSE; // used as a flag to indicate if a correct process name was found

    while(j < MAX_ENTRIES){
        if(strcmp(prc, flist[j].name) == 0){
            prcMatch = TRUE;
            j = j + MAX_ENTRIES; // indicates to the while loop that we have found the right function and can leave
    }
        else
            j++;
    }
    j = j - MAX_ENTRIES; // returns j to the appropriate command table index

    // here we will either call the appropriate command, or output error '?' symbol
    if (prcMatch == FALSE){
        ;
    }
    else { // at this point wehave a valid process name. this is where we will do reg proc stuff

        while(i < strlen(displayStr)){ // outputs added to list
            send_msg(displayStr[i], MONsrc, UARTq); // says yo we got it
            i++;
        }

        send_msg('\n', MONsrc, UARTq);

        createPCB(j, pri, pID); // send in function pointer here


    }

}

void start(){
    running = running->next;

   // printList();
    // here we will start the first process
    SVC();
}



/*
 *          The following functions serve to create our PCBS and add them to our linked list
 *
 */

void createPCB(int whichPrc, int pr, int id) {

    unsigned long *stkTop; // points to the top of our new process stack
    struct stack_frame *newStk; // points to the top of our register and special reg stack frame in hi memory in our stack

    struct pcb *newPCB = (struct pcb *)malloc(sizeof(struct pcb)); // creates a pointer to out new PCB

    llCount++;

    stkTop = (unsigned long *)malloc(PSTACKSZ * sizeof(unsigned long)); // creates a pointer to the top of our new process stack of size PSTACKSZ

    // directs our new stack frame pointer (will be this PCBs PSP) to the appopriate spot in the code
    newStk = (struct stack_frame*)(stkTop + (PSTACKSZ - sizeof(struct stack_frame)));


    newStk->r0 = 0x00000000; // this needs to be initialized to zero
    newStk->pc =  (unsigned long)flist[whichPrc].func; // assigns the entry point to the process
    newStk->psr = THUMBMODE; // indicates that the cortex is operating in thumb mode

   // newPCB->sp = (unsigned long)newStk;
    newPCB->sp = newStk; // now that sp is a pointer to stackframe, we don't need to cast
    newPCB->top_of_stack = stkTop;

    newPCB->id = id;
    newPCB->next = NULL;
    newPCB->prev = NULL;

    addPCB(newPCB);
}


void addPCB(struct pcb *PCB) {
    //empty condition
    if (running == NULL) {
        running = PCB;
        running->next = running ->prev = running;
        ll_head = running;
        //printf("henlo\n");
    }
    else {
        PCB->next = ll_head;
        PCB->prev = running;
        running->next = PCB;
        ll_head->prev = PCB;
       // running->prev= PCB;
        running = PCB;
        //printf("frenn\n");
    }
}


/*
 *          The following functions are our processes context switching function
 *          these were provided to us by
 *
 * */

unsigned long get_PSP(void)
{
/* Returns contents of PSP (current process stack */
__asm(" mrs     r0, psp");
__asm(" bx  lr");
return 0;   /***** Not executed -- shuts compiler up */
        /***** If used, will clobber 'r0' */
}

unsigned long get_MSP(void)
{
/* Returns contents of MSP (main stack) */
__asm(" mrs     r0, msp");
__asm(" bx  lr");
return 0;
}

void set_PSP(volatile unsigned long ProcessStack)
{
/* set PSP to ProcessStack */
__asm(" msr psp, r0");
}

void set_MSP(volatile unsigned long MainStack)
{
/* Set MSP to MainStack */
__asm(" msr msp, r0");
}

void volatile save_registers()
{
/* Save r4..r11 on process stack */
__asm(" mrs     r0,psp");
/* Store multiple, decrement before; '!' - update R0 after each store */
__asm(" stmdb   r0!,{r4-r11}");
__asm(" msr psp,r0");
}

void volatile restore_registers()
{
/* Restore r4..r11 from stack to CPU */
__asm(" mrs r0,psp");
/* Load multiple, increment after; '!' - update R0 */
__asm(" ldmia   r0!,{r4-r11}");
__asm(" msr psp,r0");
}

unsigned long get_SP()
{
/**** Leading space required -- for label ****/
__asm("     mov     r0,SP");
__asm(" bx  lr");
return 0;
}

/*
 *      These are kernel functions that can be indirectly called within processes
 *      see in SVCHandler for when they get called
 */

void k_terminater()
{
    // here we want to deallocate the stack and the pcb of the running process

    struct pcb *temp;


    if(running != ll_head){
        running->prev->next = running->next;
        running->next->prev = running ->prev;

    }
    else{
        //send_msg('x', MONsrc, UARTq);
        ll_head->prev->next = running->next;
        running->next->prev = ll_head->prev;
        ll_head = running->next;
    }
 free(running->top_of_stack);
    temp = running;



  //  struct pcb *temp = running;
   // running = temp->next;
  //  running = running->prev;
    free(temp);

    // we will set our new psp
   // set_PSP((unsigned long)running->sp);

        //finally lets restore our registers
   // restore_registers();

   // char strii[3];

    //sprintf(strii, "%d", running->id);

    //send_msg(strii[0], MONsrc, UARTq);

  //  printList();
   // send_msg('n', MONsrc, UARTq);

}

void k_nice_caller()
{



}







/*
 *
 *      functions that are used for debugging purposes
 *
 */

void printList(void)
{
    int i = 0; // facilitates sending the output strings
    int k = 0; // facilitates moving to the next PCB

    char strA [300];
    char strB [200];

    while (k < llCount) {

       unsigned long  r0_ptr = ((unsigned long)running->sp + (unsigned long)(R0_OFFSET * sizeof(unsigned long))); // this points to our r0 location in memory
       unsigned long  pc_ptr = ((unsigned long)running->sp + (unsigned long)(PC_OFFSET * sizeof(unsigned long))); // this points to our pc location in memory
       unsigned long  psr_ptr = ((unsigned long)running->sp + (unsigned long)(PSR_OFFSET * sizeof(unsigned long))); // this points to our psr location in memory

       struct stack_frame *spp = (running->sp); // this points to the running processes stack_fram struct

       sprintf(strA, "\n\n---------------------\n\nLinked List Entry:    %d\nProcess ID:     %d\n\nreg  @   mem loc     =   value", k+1, running->id);
       sprintf(strB, "\n\nr0   @   %u   =   %u \npc   @   %u   =   %u \npsr  @   %u   =   %u", r0_ptr, (unsigned long)spp->r0, pc_ptr, (unsigned long)spp->pc, psr_ptr, (unsigned long)spp->psr );

        while(i < strlen(strA)){
            send_msg(strA[i], MONsrc, UARTq); // sends the process title string
           i++;
        } i = 0;

        while(i < strlen(strB)){
            send_msg(strB[i], MONsrc, UARTq); // sends the registers and their values
            i++;
        } i = 0;

        running = running->next; // prints out our first one first

         k++;
     }
}
