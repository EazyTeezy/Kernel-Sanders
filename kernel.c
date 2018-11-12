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
#include "ISRs.h"
#include "MSGpasser.h"
/*
 *
 *
 *
 *
 *                                  Globals
 *
 *
 *
 *
 * */

/* here is where the 3 different process table entries are built */
struct fentry flist[MAX_ENTRIES] = { {proc1, "P1", 8}, {proc2, "P2", 8}, {proc3, "P3", 8}, {proc4, "P4", 8}, {proc5, "P5", 8}, {proc6, "P6", 8}, {proc7, "P7", 8}, {proc8, "P8", 8}, {proc9, "P9", 8}, {proc10, "P10", 8},{time_server, "TS", 8}, {monitor, "MN", 8}, {idle_proc, "IDL", 8}};

//struct pcb *running = NULL;
//struct pcb *ll_head = NULL;
struct pcb *ll_head[MAX_PRIORITIES] = {NULL, NULL, NULL, NULL, NULL, NULL};
struct pcb *running[MAX_PRIORITIES] = {NULL, NULL, NULL, NULL, NULL, NULL};

int high_priority = 0;

/* this is where the list of message queue information blocks (MQ_list_Entry) reside.All 'inUse' set = FALSE*/
//struct MQ_List_Entry MQ_list[MAX_MAILBOXES] = {{NULL, NULL, FALSE, -1}, {NULL, NULL, FALSE, -1}, {NULL, NULL, FALSE, -1}, {NULL, NULL, FALSE, -1}, {NULL, NULL, FALSE, -1}, {NULL, NULL, FALSE, -1}, {NULL, NULL, FALSE, -1}, {NULL, NULL, FALSE, -1}, {NULL, NULL, FALSE, -1}, {NULL, NULL, FALSE, -1},}; // this is our list of message queues. each queue can be bound to a specific process
struct MQ_List_Entry MQ_list[MAX_MAILBOXES]; // instead of the above method, a function now initializes our values

// by calling bind()


struct MQ_Item * freeMQblock; // used in the meemory allocation of MQ message blocks

int llCount = 0; // the number of items in our linked list (used for debugging purposes)





/*
 *
 *                                          reg_proc() and start()
 *
 * */

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
          //  send_msg(displayStr[i], MONsrc, UARTq); // says yo we got it
            i++;
        }

        createPCB(j, pri, pID); // send in function pointer here


    }

}

void start(){

    running[high_priority] = running[high_priority]->next; // shifts our running pcb to the right, meaning the first entered process is the first to run

    createMQblocks();

    init_MQ_list();
   // printList();

  // here we will start the first process
    SVC();
}

void createMQblocks(){ // allocates memory for the message blocks

    int i = 0; // loop counter

    // struct MQ_Item * freeMQblock; is defined as a global variable
    struct MQ_Item * prevMQblock;

    while(i < 256){ // we want to allocate memory for 256 message blocks


        freeMQblock = malloc(sizeof(struct MQ_List_Entry));

        if(!i) // if the first memory allocation
            freeMQblock->next = NULL; // the end of the allocated memory blocks
        else
            freeMQblock->next = prevMQblock; // connecting the linked list

        prevMQblock = freeMQblock;

        i++; // increment while loop counter (so we have enough space for 256 messages stored at once)
    }
}

void init_MQ_list(){

    int i = 0; // loop counter for initializing individual indexed list pointers

    while( i < (MAX_MAILBOXES - 2) ){ // minus 2 as the last two mailboxes will be reserved for the UART

        MQ_list[i].oldest = NULL;
        MQ_list[i].youngest = NULL;
        MQ_list[i].inUse = FALSE;
        MQ_list[i].owner = -1; // -1 is a default

        i++;
    }

    /* Initializing the to monitor queue */

    MQ_list[i].oldest = NULL;
    MQ_list[i].youngest = NULL;
    MQ_list[i].inUse = TRUE;
    MQ_list[i].owner = TO_UART; // reserved for sending messages to the monitor

    i++; // moving us to the last entry in MQ_list[i]

    MQ_list[i].oldest = NULL;
    MQ_list[i].youngest = NULL;
    MQ_list[i].inUse = TRUE;
    MQ_list[i].owner = TO_MON; // // reserved for sending messages to the monitor process from the UART
}


/*
 *
 *
 *
 *
 *
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
 *
 *
 *      These are kernel functions that can be indirectly called within processes
 *      see in SVCHandler for when they get called
 *
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
    int samePriority = 0;
    //trying to nice something to it's current priority - won't do anything
    if(high_priority == new_priority)
        samePriority = 1;

    struct pcb *temp; // used when fixing the connections of our linked lists
if(!samePriority){
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
       if(high_priority > new_priority)
           set_PSP((unsigned long)running[high_priority]->sp);
       free(temp);
    }
}


}
int k_bind(int indx)
{
    int succesfulBind; // succesful or unsucessful flag


    if(MQ_list[indx].inUse == FALSE){ // if the MQ_list
        MQ_list[indx].inUse = TRUE;
        MQ_list[indx].owner = running[high_priority]->id;
        succesfulBind = TRUE; //
    }
    else
        succesfulBind = FALSE;//  we return an error (failed to bind flag)

    return succesfulBind;
}


/*
 *
 *
 *                 kernel-level message passing  support routines
 *
 * */
int k_send(int to_dst, int from, char * msg, int size)
{

    int succesfulSend; // dont actually initilize this to a value
    if(!MQ_list[to_dst].inUse){ // if the destination mailbox hasnt been binded to
        succesfulSend = FALSE;
    }
    else if(MQ_list[to_dst].owner != to_dst) // if the destination mailbox is owned by the wrong address // this is iffy reasoning figure it out
        succesfulSend = FALSE;

    else{

        struct MQ_Item * MQ_Item_Ptr; // pointer to our new entry in the message queue

        char * k_msg_ptr; // points to the messsage in k_space
        char * p_msg_ptr; // points to the message in p_space
        int psz = size; // equal to the size sent in from p_space

        MQ_Item_Ptr = MQmalloc();
        // null check


        MQ_Item_Ptr -> size = psz; // sets the size to the value sent from p_space

        p_msg_ptr = msg;
        k_msg_ptr = MQ_Item_Ptr -> msg;

        // now mem copy *pptr into *kptr
        memcpy(k_msg_ptr, p_msg_ptr, strlen(p_msg_ptr)); // SHOULD THIS STRLEN() BE PPTR OR KPTR

        //add MQ_Item_Ptr to list
        if(MQ_list[to_dst].youngest == NULL){ // if the list is empty
            MQ_Item_Ptr -> next = NULL; // our new youngests 'next pointer' will point to NULL
           //send_msg('h', MONsrc, UARTq);
            MQ_list[to_dst].youngest = MQ_Item_Ptr;
            MQ_list[to_dst].oldest = MQ_Item_Ptr;
        }
        else{ // if there are already waiting messages in the queue
            MQ_Item_Ptr -> next = NULL; // our new youngests 'next pointer' will point to NULL

            if(MQ_list[to_dst].youngest == MQ_list[to_dst].oldest){ // if there is only one message block in the list
                MQ_list[to_dst].oldest -> next = MQ_Item_Ptr; // our oldest 'next pointer' gets set to our new youngest message
                MQ_list[to_dst].youngest = MQ_Item_Ptr; // sets our new Item as the youngest
            }
            else{ // if there are at least 2 entries, the following linked list manipulation occurs
                MQ_list[to_dst].youngest -> next  = MQ_Item_Ptr; // sets the old youngest next equal to our newest message block
                MQ_list[to_dst].youngest = MQ_Item_Ptr; // sets our new Item as the youngest
            }
        }

    }

    return succesfulSend;
}



int k_recv(int me, int * src, char * newMsg, int sz)
{
   // UART_sendChar('u');
   // char strA[10];
   // char strB[10];

    int succesfulRecv; // dont actually initilize this to a value

    if(MQ_list[me].owner == NULL){
        succesfulRecv = FALSE;// if the calling processes mailbox hasnt been binded to
    }
    else if(MQ_list[me].owner != running[high_priority]->id && MQ_list[me].owner != TO_MON){ // the && MQ_list[me].owner != TO_MON is for the monitor case

        succesfulRecv = FALSE;// if the calling processes mailbox is improperly owned
    }
    else if(MQ_list[me].youngest == NULL){ // if the calling processes mailbox has no message waiting

        succesfulRecv = FALSE;// if the calling processes mailbox has no message waiting
    }
    else{

        succesfulRecv = TRUE;

        char * k_msg_ptr; // points to the messsage in k_space
        char * p_msg_ptr; // points to the message in p_space

        int * k_msgSrc_ptr;
        int * p_msgSrc_ptr;

        struct MQ_Item * MQ_Item_Ptr;
        struct MQ_Item * temp;



        MQ_Item_Ptr = MQ_list[me].oldest; // our MQ_item_Ptr now points to the oldest message in the list -- which will be the message to be received by the caller

        p_msgSrc_ptr = src;     // the pointer to the memory location in the callers stack where the source info will be stored
        k_msgSrc_ptr = MQ_Item_Ptr -> src; // the pointer to the memory location in the msp where the source of the received message is stored

        // now mem copy *p_msgSrc_ptr into *k_msgSrc_ptr
       // memcpy(p_msgSrc_ptr, k_msgSrc_ptr, strlen(k_msgSrc_ptr)); // SHOULD THIS STRLEN() BE PPTR OR KPTR


       // *p_msgSrc_ptr = *k_msgSrc_ptr; // not sure how to get this working

        p_msg_ptr = newMsg;
        k_msg_ptr = MQ_Item_Ptr -> msg;

        // now mem copy *p_msg_ptr into *k_msg_ptr
        memcpy(p_msg_ptr, k_msg_ptr, strlen(p_msg_ptr)); // SHOULD THIS STRLEN() BE PPTR OR KPTR

        //add MQ_Item_Ptr to list

        /*
         *
         *          Have to set up for a case where the received message is the last message. (some queue stuff)
         *
         * */

        if(MQ_list[me].oldest == MQ_list[me].youngest){ // if it is the last message in the queue

            MQfree(MQ_list[me].oldest); // free it

            MQ_list[me].oldest = NULL; // set the oldest/youngest pointers equal to NULL
            MQ_list[me].youngest = NULL;
        }
        else{
             temp = MQ_list[me].oldest;
             MQ_list[me].oldest = MQ_list[me].oldest -> next;

             MQfree(temp);

        }
    }

    return succesfulRecv;
}


int k_uart_organizer(char ch, char cmdchar, char col1, char col2, char esc, char line1, char line2, char semicolon, char sqrbrkt){

    //int i = 0; // counter variable

    //char CUPandMSG[9] = {esc, sqrbrkt, line1, line2, semicolon, col1, col2, cmdchar, ch}; // creates a copy of our ch message locally (to be poined at)

    k_uart_send(esc);
    k_uart_send(sqrbrkt);
    k_uart_send(line1);
    k_uart_send(line2);
    k_uart_send(semicolon);
    k_uart_send(col1);
    k_uart_send(col2);
    k_uart_send(cmdchar);
    k_uart_send(ch); // ch

/*
    while(i < 9){

        k_uart_send(CUPandMSG[i]);
        i++;
    } i = 0;
*/
    return 1;

}

int k_uart_send(char output)
{
    int i = 0; // counter variable
    int succesfulSend; // dont actually initilize this to a value

    char data = output; //



    if(UartOutIdle == TRUE){ // if the UART is not busy

       UART_sendChar(data);
       i++;

        succesfulSend = TRUE;
    }
    else{ // if the UART is busy, we will store the message and its poisitoin details in a linked list

        struct MQ_Item * MQ_Item_Ptr; // pointer to our new entry in the message queue

        char * k_msg_ptr; // points to the messsage in k_space
        char * p_msg_ptr; // points to the message in p_space // dont think we need this in this case as we have a copy of our ch
        int psz = ONE_CHAR; // equal to the size sent in from p_space

        MQ_Item_Ptr = MQmalloc();
        // null check


        MQ_Item_Ptr -> size = psz; // sets the size to the value sent from p_space

        p_msg_ptr = &data;
        k_msg_ptr = MQ_Item_Ptr -> msg;

        // now mem copy *pptr into *kptr
        memcpy(k_msg_ptr, p_msg_ptr, strlen(p_msg_ptr)); // SHOULD THIS STRLEN() BE PPTR OR KPTR

        //add MQ_Item_Ptr to list
        if(MQ_list[TO_UART].youngest == NULL){ // if the list is empty
            MQ_Item_Ptr -> next = NULL; // our new youngests 'next pointer' will point to NULL
           //send_msg('h', MONsrc, UARTq);
            MQ_list[TO_UART].youngest = MQ_Item_Ptr;
            MQ_list[TO_UART].oldest = MQ_Item_Ptr;
        }
        else{ // if there are already waiting messages in the queue
            MQ_Item_Ptr -> next = NULL; // our new youngests 'next pointer' will point to NULL

            if(MQ_list[TO_UART].youngest == MQ_list[TO_UART].oldest){ // if there is only one message block in the list
                MQ_list[TO_UART].oldest -> next = MQ_Item_Ptr; // our oldest 'next pointer' gets set to our new youngest message
                MQ_list[TO_UART].youngest = MQ_Item_Ptr; // sets our new Item as the youngest
            }
            else{ // if there are at least 2 entries, the following linked list manipulation occurs
                MQ_list[TO_UART].youngest -> next  = MQ_Item_Ptr; // sets the old youngest next equal to our newest message block
                MQ_list[TO_UART].youngest = MQ_Item_Ptr; // sets our new Item as the youngest
            }
        }
    }

    succesfulSend = TRUE;

    return succesfulSend;
}

int k_uart_recv(char* d, int* s)
{

    int succesfulRecv; // dont actually initilize this to a value

    if(MQ_list[TO_UART].youngest == NULL){ // if the calling processes mailbox has no message waiting

        succesfulRecv = FALSE;// if the calling processes mailbox has no message waiting
    }
    else{
        succesfulRecv = TRUE;

        char * k_msg_ptr; // points to the messsage in k_space
        char * p_msg_ptr; // points to the message in p_space

        struct MQ_Item * MQ_Item_Ptr;
        struct MQ_Item * temp;



        MQ_Item_Ptr = MQ_list[TO_UART].oldest; // our MQ_item_Ptr now points to the oldest message in the list -- which will be the message to be received by the caller


        // now mem copy *p_msgSrc_ptr into *k_msgSrc_ptr
       // memcpy(p_msgSrc_ptr, k_msgSrc_ptr, strlen(k_msgSrc_ptr)); // SHOULD THIS STRLEN() BE PPTR OR KPTR


        *d = *(MQ_Item_Ptr -> msg); // not sure how to get this working


        //add MQ_Item_Ptr to list

        /*
         *
         *          Have to set up for a case where the received message is the last message. (some queue stuff)
         *
         * */

        if(MQ_list[TO_UART].oldest == MQ_list[TO_UART].youngest){ // if it is the last message in the queue

            MQfree(MQ_list[TO_UART].oldest); // free it

            MQ_list[TO_UART].oldest = NULL; // set the oldest/youngest pointers equal to NULL
            MQ_list[TO_UART].youngest = NULL;
        }
        else{                                           // if there are more messages in the queue

             temp = MQ_list[TO_UART].oldest;
             MQ_list[TO_UART].oldest = MQ_list[TO_UART].oldest -> next;

             MQfree(temp);

        }
    }

    return succesfulRecv;
}

struct MQ_Item * MQmalloc(){

    struct MQ_Item * MQp = freeMQblock;

    freeMQblock = freeMQblock->next;

    return MQp;
}

void MQfree(struct MQ_Item * MQp){

    MQp->next = freeMQblock;
    freeMQblock = MQp;

}


/*
 *
 *
 *
 *          The following functions are our processes context switching function
 *          these were provided to us by
 *
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
