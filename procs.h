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


// process prototypes
void proc1();
void proc2();
void proc3();
void idle_proc();

// functions
int get_id();
void terminate();

void assignR7(volatile unsigned long data);

#endif /* PROCS_H_ */