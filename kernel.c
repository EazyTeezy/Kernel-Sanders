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

//struct pcb *running = NULL;
//struct pcb *ll_head = NULL;
struct pcb *ll_head[MAX_PRIORITIES] = {NULL, NULL, NULL, NULL, NULL, NULL};
struct pcb *running[MAX_PRIORITIES] = {NULL, NULL, NULL, NULL, NULL, NULL};

int high_priority = 0;


struct MQ_T MQ_list[MAX_PROCS]; // this is our list of message queues. each queue can be bound to a specific process
// by calling bind()


int llCount = 0; // the number of items in our linked list (used for debugging purposes)

void reg_proc(char * prc, int pri, int pID){ // this is our argument)

    int j = 0; // used when searching unique processes table for matching name
    int i = 0; // used when outputting the matched process name
    char displayStr[23] = "\nProcess Registered..."; // sending in a '\n' here messes everything up with p1

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

        while(i < strlen(displayStr)){ // outputs "Process Registered..."
            send_msg(displayStr[i], MONsrc, UARTq); // says yo we got it
            i++;
        }

        createPCB(j, pri, pID); // send in function pointer here


    }

}

void start(){
    running[high_priority] = running[high_priority]->next; // shifts our running pcb to the right, meaning the first entered process is the first to run

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

    addPCB(newPCB, pr);
}


void addPCB(struct pcb *PCB, int prio) {

    if(prio > high_priority)
        high_priority = prio;

    //empty condition
    if (running[prio] == NULL) {
        running[prio] = PCB;
        running[prio]->next = running[prio] ->prev = running[prio];
        ll_head[prio] = running[prio];
        //printf("henlo\n");
    }
    else {
        PCB->next = ll_head[prio];
        PCB->prev = running[prio];
        running[prio]->next = PCB;
        ll_head[prio]->prev = PCB;
       // running->prev= PCB;
        running[prio] = PCB;
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

    if(running[high_priority]->next == running[high_priority]){ // check to see if this is the priorities last process being killed
        free(running[high_priority]->top_of_stack);
        free(running[high_priority]);
        running[high_priority] = NULL;
    }
    else{ // if its not the last priority, we'll need to fix the connection of the leftover adjacent nodes

       if(running[high_priority] != ll_head[high_priority]){
           running[high_priority]->prev->next = running[high_priority]->next;
           running[high_priority]->next->prev = running[high_priority] ->prev;
       }
       else{
           ll_head[high_priority]->prev->next = running[high_priority]->next;
           running[high_priority]->next->prev = ll_head[high_priority]->prev;
           ll_head[high_priority] = running[high_priority]->next;
       }

       free(running[high_priority]->top_of_stack);
       temp = running[high_priority];
       running[high_priority] = temp->next;
       free(temp);
    }


    while(!running[high_priority]){ // if all processes at this priority have terminated, move down a priority until a non empty priority level is found
            high_priority--;
    }

    // we will set our new psp
    set_PSP((unsigned long)running[high_priority]->sp);

    // note: the registers are then restored in SVCall
}

void k_nice(int new_priority)// here is where we will remove the running PCB from the current priority linked list, and tack it onto the end of another list
{
    //trying to nice something to it's current priority - won't do anything
    if(high_priority == new_priority)
        return;

    struct pcb *temp; // used when fixing the connections of our linked lists

    if(running[high_priority]->next == running[high_priority]){ //if last item
        //Reset the previously linked list contents and assign current running process the SP
        running[high_priority]-> next = running[high_priority]->prev = NULL;
        running[high_priority]->sp = (struct stack_frame*)get_PSP();
        //Add the PCB to the new priority list
        addPCB(running[high_priority],new_priority);
        //list is empty - running points at NULL
        running[high_priority] = NULL;
        // we will then set our new psp
          while(!running[high_priority]) // if all processes at this priority have terminated, move down a priority until a non empty priority level is found
              high_priority--;

          set_PSP((unsigned long)running[high_priority]->sp);
    }
    else{
        //Remove PCB from current priority queue
       if(running[high_priority] != ll_head[high_priority]){ // if its not the last element in this priority
           running[high_priority]->prev->next = running[high_priority]->next;
           running[high_priority]->next->prev = running[high_priority] ->prev;
       }
       else{
           ll_head[high_priority]->prev->next = running[high_priority]->next;
           running[high_priority]->next->prev = ll_head[high_priority]->prev;
           ll_head[high_priority] = running[high_priority]->next;
       }
       if(high_priority >= new_priority)
           temp = running[high_priority] -> next;
       else
           temp = running[high_priority];

       //Reset the previously linked list contents and assign current running process the SP
       running[high_priority]-> next = running[high_priority]->prev = NULL;
       running[high_priority]->sp = (struct stack_frame*)get_PSP();
       //Add the PCB to the new priority list
       addPCB(running[high_priority],new_priority);
       //point to the next item - stored as temp
       running[high_priority] = temp;
       if(high_priority >= new_priority)
           set_PSP((unsigned long)running[high_priority]->sp);
       free(temp);
    }


    //else {
        //send_msg('z', MONsrc, UARTq);// send 1s
      //  while(!running[high_priority]) // if all processes at this priority have terminated, move down a priority until a non empty priority level is found
        //    high_priority--;
  //  }


}
void k_bind(int indx)
{
    if(MQ_list[indx].avail == TRUE){
        MQ_list[indx].avail = FALSE;
        MQ_list[indx].owner = running[high_priority]->id;
    }


}






/*
 *
 *      functions that are used for debugging purposes
 *
 */

void printList(void) // out of date, has not been update to handle priority list
{
  /*  int i = 0; // facilitates sending the output strings
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
     */
}
