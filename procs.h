/*
 * procs.h
 *
 *  Created on: Oct 12, 2018
 *      Author: NickB
 */

#ifndef PROCS_H_
#define PROCS_H_

#define PROC1_DST   1 // these definitions are used when developing message passing
#define PROC2_DST   2

#define PROC1_SRC   10
#define PROC2_SRC   20

#define ESC         27

#define COL_ONE     1
#define ROW_ONE     1

#define BACKSPACE       0x7f

#define DEMO_MSG_SIZE   5


// process prototypes
void proc1();
void proc2();
void proc3();
void proc4();
void proc5();
void proc6();
void proc7();
void proc8();
void proc9();
void proc10();

void time_server(); //
void monitor(); //

void idle_proc();

// functions

void pkcall(int code, void * structurePtr); // used to handle pkcalls with no returns

int pkcall_with_return(int code, void * structurePtr); // used to handle pkcalls with returns

int p_get_id(); // return value is the id
void p_terminate();
void p_nice(int incr);
int p_bind(int index); // return value is TRUE for succesful bind and FALSE for unsuccesful bind

int p_send(int to, int from, char * msg, int size);
int p_recv(int my_mailbox, int * from, char * msg, int sz);

int uart_output_ch(int row, int col, char ch);

void p_assignR7(volatile unsigned long data);

#endif /* PROCS_H_ */
