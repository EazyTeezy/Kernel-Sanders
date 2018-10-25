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


void proc1()
{
    int i = 0; // delay loop counter

   // while(1){

        while(i < 1000000){
              i++;
         }
        send_msg('1', MONsrc, UARTq);// send 1s
        i = 0; // resets delay loop counter
    //}
        terminate();

        while(1);

}

void proc2()
{


    int j = 0;// delay loop counter


  //  while(1){

        while(j < 1000000){
            j++;
        } j = 0;  // resets delay loop counter

        send_msg('2', MONsrc, UARTq);// send 2s
 //   }
        terminate();
        while(1);
}

void proc3() // currently is the get_id process
{
    int myid;

    myid = get_id();

    char strID [1];
    sprintf(strID, "%d", myid);

    int i = 0; // delay loop counter

   // while(1){
       while(i < 1000000){
            i++;
       }
        send_msg(strID[0], MONsrc, UARTq);
        i = 0; // resets delay loop counter
 //   }

    terminate(); // the function which will make the kernel command call
   // send_msg('x', MONsrc, UARTq);
  //  SysTickIntDisable();
    while(1);

}

void idle_proc() // currently is the get_id process
{
    while(1);
}


/*
 *
 *  Process functions
 *
 */

int get_id()
{

    volatile struct kcallargs getidarg; /* Volatile to actually reserve space on stack*/
    getidarg.code = GETID;

    /* Assign address of getidarg to R7 */
    assignR7((unsigned long) &getidarg);

    SVC();

    return getidarg.rtnvalue;

}

void terminate()
{

    volatile struct kcallargs getidarg; /* Volatile to actually reserve space on stack*/
    getidarg.code = TERMINATE;

    /* Assign address of getidarg to R7 */
    assignR7((unsigned long) &getidarg);


    SVC();

}

void assignR7(volatile unsigned long data)
{
    /* Assign 'data' to R7; since the first argument is R0, this is
     *simply a move from R0 to R7
     */

    __asm(" mov r7,r0");
}
