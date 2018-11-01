/*
 * procs.c
 *
 *  Created on: Oct 12, 2018
 *      Author: NickB
 */


#include "procs.h"
#include "MSGpasser.h"
#include "kernelcalls.h"
#include "kernel.h"
#include "ISRs.h"

#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

int nice_flag = 0;

void proc1() // During development this process acts primarily as a never ending process which prints 1s
{
    //int i = 0; // delay loop counter
    int worked;
    char * p1message = "12345";

    p_send(2, 1, p1message, 5);


    worked = p_bind(1);

    if(worked)
        send_msg('s', MONsrc, UARTq);
    else
        send_msg('f', MONsrc, UARTq);


/*while(1){

        while(i < 100000){
              i++;
         }
      */  send_msg('1', MONsrc, UARTq);// send 1s
      /*  i = 0; // resets delay loop counter

 //  send_msg('x', MONsrc, UARTq);// send 1s
   //
        if(!nice_flag)
         p_nice(1);
       // p_terminate();
    }*/
    //while(1){

   // }
      p_terminate();
}

void proc2()
{

    int j = 0;// delay loop counter

    int incomingMsgSrc; // memory location on this processeses stack for incoming message source
    char incomingMsg[5]; // memory location on this processeses stack for incoming message

    int worked;

    worked = p_bind(2);
  //  while(1){

    if(worked)
        send_msg('s', MONsrc, UARTq);
    else
        send_msg('f', MONsrc, UARTq);


    p_recv(6, &incomingMsgSrc, &incomingMsg, 5); // 5 because thats what the incoming message is

               while(j < 10000){
            j++;
        } j = 0;  // resets delay loop counter

        send_msg('2', MONsrc, UARTq);// send 2s
 //   }
       // p_nice(3);
        p_terminate();

}

void proc3() // currently is the get_id process
{
    int myid;

    myid = p_get_id();

    char strID [1];
    sprintf(strID, "%d", myid);

    int i = 0; // delay loop counter

   // while(1){
       while(i < 10000){
            i++;
       }
        send_msg(strID[0], MONsrc, UARTq);
        i = 0; // resets delay loop counter
 //   }

    p_terminate(); // the function which will make the kernel command call

}

void idle_proc() // currently is the get_id process
{
    int i = 1; // loading bar count variable
    int j = 0; // delay loop counter

    while(1){

        while(j < 1000000){
                      j++;
        }

        if(i == 1){
            send_msg(' ', MONsrc, UARTq);
        }
        else if(i == 2){
            send_msg('i', MONsrc, UARTq);
        }
        else if(i == 3){
            send_msg('d', MONsrc, UARTq);
        }
        else if(i == 4){
            send_msg('l', MONsrc, UARTq);
        }
        else if(i == 5){
            send_msg('e', MONsrc, UARTq);
        }
        else if(i == 6){
            send_msg(' ', MONsrc, UARTq);
        }
        else if(i == 7){
            send_msg('.', MONsrc, UARTq);
        }
        else if(i == 8){
            send_msg('.', MONsrc, UARTq);
        }
        else if(i == 9){
            send_msg('.', MONsrc, UARTq);
            i = 0; // resets loading bar switch variable
        }

        i++; // increments loading symbol counter
        j = 0; // resets delay variable
    }
}





/*
 *
 *  Process functions
 *
 */

int p_get_id()
{

    volatile struct kcallargs getidarg; /* Volatile to actually reserve space on stack*/
    getidarg.code = GETID;

    /* Assign address of getidarg to R7 */
    p_assignR7((unsigned long) &getidarg);

    SVC();

    return getidarg.rtnvalue;

}

void p_terminate()
{

    volatile struct kcallargs terminateArg; /* Volatile to actually reserve space on stack*/
    terminateArg.code = TERMINATE;

    /* Assign address of getidarg to R7 */
    p_assignR7((unsigned long) &terminateArg);


    SVC();

}

void p_nice(int incr)
{

    volatile struct kcallargs niceArg; /* Volatile to actually reserve space on stack*/
    niceArg.code = NICE;
    niceArg.arg1 = incr;

    /* Assign address of getidarg to R7 */
    p_assignR7((unsigned long) &niceArg);
    //nice_flag = 1;
    SVC();

}

int p_bind(int index) // return value used as success or failure flag
{

    volatile struct kcallargs bindArg; /* Volatile to actually reserve space on stack*/
    bindArg.code = BIND;
    bindArg.arg1 = index;

    /* Assign address of getidarg to R7 */
    p_assignR7((unsigned long) &bindArg);

    SVC();

    return bindArg.rtnvalue;

}

int p_send(int to, int from, char * msg, int size){

    volatile struct kcallargs sendArg; /* Volatile to actually reserve space on stack*/
    sendArg.code = SEND;
    sendArg.arg1 = to;
    sendArg.arg2 = from;
    sendArg.arg3 = msg;
    sendArg.arg4 = size;

        /* Assign address of getidarg to R7 */
        p_assignR7((unsigned long) &sendArg);

        SVC();

        return sendArg.rtnvalue;

}

int p_recv(int my_mailbox, int * from, char * msg, int sz){

    volatile struct kcallargs recvArg; /* Volatile to actually reserve space on stack*/
    recvArg.code = RECV;
    recvArg.arg1 = my_mailbox;
    recvArg.arg5 = from;
    recvArg.arg3 = msg;
    recvArg.arg4 = sz;

            /* Assign address of getidarg to R7 */
    p_assignR7((unsigned long) &recvArg);

    SVC();

    return recvArg.rtnvalue;
}



void p_assignR7(volatile unsigned long data)
{
    /* Assign 'data' to R7; since the first argument is R0, this is
     *simply a move from R0 to R7
     */

    __asm(" mov r7,r0");
}
