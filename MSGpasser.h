/*
 * MSGpasser.h
 *
 *  Created on: Oct 12, 2018
 *      Author: NickB
 */

#ifndef MSGPASSER_H_
#define MSGPASSER_H_

#define MAX_Q_SIZE  4096
#define EMPTY_FLAG  -1
#define MAX_ENTRIES  13

#define MONq    0 // these are used to indicate what Q to enter, or from where it has come
#define UARTq    1


#define toMON_or_toUART     2
#define NUM_OF_QUEUES       2

/* this struct holds the message components,
 * including the source (UART or SYSCLCK) and the
 *  data (char from user keystroke or time signal) */
struct message
{
    char msg; /* letter/symbol or timing tick */
    int src; /* UART or SYSTICK*/
};



void send_msg(char msgData, int source, int outQueue);
int rec_msg(char* d, int* s, int inQueue); // this routine returns aa struct with the message information or 0 depending on if the
                          //toMonitorQ (elevator Q) has a message or is empty



#endif /* MSGPASSER_H_ */
