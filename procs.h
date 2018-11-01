/*
 * procs.h
 *
 *  Created on: Oct 12, 2018
 *      Author: NickB
 */

#ifndef PROCS_H_
#define PROCS_H_

#define MONq    0 // these are used to indicate what Q to enter
#define UARTq    1

#define MONsrc      3

#define PROC1_DST   1 // these definitions are used when developing message passing
#define PROC2_DST   2

#define PROC1_SRC   10
#define PROC2_SRC   20


// process prototypes
void proc1();
void proc2();
void proc3();
void idle_proc();

// functions
int p_get_id(); // return value is the id
void p_terminate();
void p_nice(int incr);
int p_bind(int index); // return value is TRUE for succesful bind and FALSE for unsuccesful bind

int p_send(int to, int from, char * msg, int size);
int p_recv(int my_mailbox, int * from, char * msg, int sz);

void p_assignR7(volatile unsigned long data);

#endif /* PROCS_H_ */
