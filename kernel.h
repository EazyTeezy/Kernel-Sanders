/*
 * kernel.h
 *
 *  Created on: Oct 12, 2018
 *      Author: NickB
 */

#ifndef KERNEL_H_
#define KERNEL_H_

void start();

#define TRUE    1
#define FALSE   0
#define PRIVATE static

#define DISPLAYTBL_LEN1          63
#define DISPLAYTBL_LEN2          90
#define MAX_ENTRIES     13 // this is the number of different unique prcesses types in our table (proc1, proc2, proc3)

#define MAX_MAILBOXES       16 // the max number of process mailboxes

#define TO_UART     14 // find a way to make this MAX_MAILBOXES - 2 rather than 8
#define TO_MON      15 // find a way to make this MAX_MAILBOXES - 1 rather than 9

#define ONE_CHAR    1
#define UART_SRC    0


#define PSTACKSZ        512
#define THUMBMODE       0x01000000

#define R0_OFFSET       8
#define PC_OFFSET       14
#define PSR_OFFSET      15

#define MAX_PRIORITIES  6

#define IDLE_PRIORITY   0
#define PRIORITY1       1
#define PRIORITY2       2
#define PRIORITY3       3
#define PRIORITY4       4
#define PRIORITY5       5




#define SVC()   __asm(" SVC #0")
#define disable()   __asm(" cpsid i")
#define enable()    __asm(" cpsie i")

#define MSP_RETURN 0xFFFFFFF9    //LR value: exception return using MSP as SP
#define PSP_RETURN 0xFFFFFFFD    //LR value: exception return using PSP as SP

void set_LR(volatile unsigned long);
unsigned long get_PSP();
void set_PSP(volatile unsigned long);
unsigned long get_MSP(void);
void set_MSP(volatile unsigned long);
unsigned long get_SP();

void volatile save_registers();
void volatile restore_registers();

/* this struct holds the addresess of the function, the name of the function, and the number of chars in it's name */
struct fentry
{
    void(*func)(); /* pointer to a function (process) */ // i think char * is ready for the argument
    char *name; /* text name associated with process */
    int size; /* max number of chars in name*/
};

void reg_proc(char * prc, int pri, int pID); // prototype for function that calls the appropraite application process

#define STACKSIZE   1024

/* Cortex default stack frame */
struct stack_frame
{
/* Registers saved by s/w (explicit) */
/* There is no actual need to reserve space for R4-R11, other than
 * for initialization purposes.  Note that r0 is the h/w top-of-stack.
 */
unsigned long r4;
unsigned long r5;
unsigned long r6;
unsigned long r7;
unsigned long r8;
unsigned long r9;
unsigned long r10;
unsigned long r11;
/* Stacked by hardware (implicit)*/
unsigned long r0;
unsigned long r1;
unsigned long r2;
unsigned long r3;
unsigned long r12;
unsigned long lr;
unsigned long pc;
unsigned long psr;
};

/* Process control block */


struct pcb
{
    /* Stack pointer - r13 (PSP) */
    struct stack_frame *sp;

    /* Pointer to the original top of allocated stack (used when freeing memory)*/
    unsigned long *top_of_stack;

    /* Processes ID */
    int id;

    /* Links to adjacent PCBs */
    struct pcb *next;
    struct pcb *prev;
};






/*
 *
 *      Message Passing Structures
 *
 * */

struct MQ_Item
{
    char msg[256];

    int * src; /* senders process id*/
    int size;
    struct MQ_Item *next;
};

struct MQ_List_Entry
{
    struct MQ_Item *youngest; // indicates the last item in the message queue
    struct MQ_Item *oldest; // indicates the first item in the message queue

    int inUse;
    int owner;


};



/* linked list stuff */
//actual linked list
void createPCB(int whichPrc, int pr, int id);
void addPCB(struct pcb*, int prio);
void printList(void);

//global variable
extern struct pcb *running[MAX_PRIORITIES];
extern struct pcb *ll_head[MAX_PRIORITIES];

extern int high_priority;



void k_terminater();
void k_nice(int new_priority);
int k_bind(int indx);

int k_send(int to_dst, int from, char * msg, int size);
int k_recv();


int k_uart_organizer(char ch, char cmdchar, char col1, char col2, char esc, char line1, char line2, char semicolon, char sqrbrkt);

int k_uart_send(char output);
int k_uart_recv(char *d, int *s);

struct MQ_Item * MQmalloc();
void MQfree(struct MQ_Item * MQp);

void createMQblocks(); //
void init_MQ_list();


#endif /* KERNEL_H_ */
