/*
 * MSGpasser.c
 *
 *  Created on: Oct 12, 2018
 *      Author: NickB
 */

#include "MSGpasser.h"
#include "ISRs.h"
#include "menu.h"

#include <string.h>

struct message Queue[toMON_or_toUART][MAX_Q_SIZE]; // this matrix of structs is used in the message passer. first element indicates what Queue, seonds is the Queues size

int front[NUM_OF_QUEUES] = {-1, -1}; // these are both arrays of two variables to indicae for which queue it is the front / rear.
int rear[NUM_OF_QUEUES] = {-1, -1};


void send_msg(char msgData, int source, int outQueue) /* this is to put our message struct objects into the array of structs (our Queue)*/
{

    if(outQueue == UARTq && UartOutIdle == TRUE) // the uart is idle, send out the character!
        UART_sendChar(msgData); // forces the first character of our string out to the UART, thus passing control to the UART
    else{ // if it is busy, we will queue the data
        if( (front[outQueue] == rear[outQueue] + 1) || (front[outQueue] == 0 && rear[outQueue] == MAX_Q_SIZE-1))  // circular Q full conditional
            /* error handling q is full */;
        else{
            if(front[outQueue] == -1)
                front[outQueue] = 0;
            rear[outQueue] = (rear[outQueue] + 1) % MAX_Q_SIZE; // circular q rear adjustment
            Queue[outQueue][rear[outQueue]].msg = msgData; // assign each individual component
            Queue[outQueue][rear[outQueue]].src = source; // ^^^^^^^^^^^^^^^^^^
        }
    }
}

int rec_msg(char* d, int* s, int inQueue)
{

    if(front[inQueue] == -1){
          *d = 0;
          *s = EMPTY_FLAG;
          return FALSE; // this queue is empty
     }
     else{

          *d = Queue[inQueue][front[inQueue]].msg; // extracting the data from the message
          *s = Queue[inQueue][front[inQueue]].src;

          if(front[inQueue] == rear[inQueue]){
              front[inQueue] = -1;
              rear[inQueue] = -1;
          } // this is if that last message was the only message left in the queue, the queue gets reset to its empty state defaults
          else{
              front[inQueue] = (front[inQueue] + 1) % MAX_Q_SIZE; // circular q front adjustment
          }
          return TRUE; // indicating we have a message -- enter the conditional and deal with it
     }
}

