/*
 * procs.c
 *
 *  Created on: Oct 12, 2018
 *      Author: NickB
 */

#include "kernelcalls.h"
#include "procs.h"
#include "MSGpasser.h"

#include "kernel.h"
#include "ISRs.h"

#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

int nice_flag = 0;

void proc1() // During development this process acts primarily as a never ending process which prints 1s
{
    int i = 0; // delay loop counter
    int SuccesfulBind;
    int SuccesfulSend;

    char * p1message = "12345"; // character pointer to a location this proesses stack mem containing the message
    SuccesfulBind = p_bind(1); // SuccesfulBind will be TRUE if succesful bind occured, FALSE if bind failed

    /*-----------------------------------------------------------------------------*/
    /* The following is an outline of the four arguments being sent                 */
    /* by this process to another process. The four arguments are as follows        */
    /*                                                                              */
    /*      arg 1: destination of the message about to be sent                      */
    /*      arg 2: message source                                                   */
    /*      arg 3: a character pointer to somewhere in this processes stacks mem    */
    /*      arg 4: the size of the message to be sent                               */
    /*
     *  note: the return value is TRUE if the data was succesfully
     *        and FALSE if the data was not able to send the message                */
    /*------------------------------------------------------------------------------*/
    if(SuccesfulBind){
        while(1){

            while(i < 1000000){
                         i++;
            }

          SuccesfulSend = p_send(2, 1, p1message, 5); // arg 1: destination of the message about to be sent

          i = 0;
        }
    }
    else
      p_terminate();
}

void proc2()
{

    int j = 0;// delay loop counter
    int k = 0;

    int incomingMsgSrc; // memory location on this processeses stack for incoming message source
    char incomingMsg[5]; // an a pointer to an array of 5 characters. memory location on this processeses stack for incoming message

    int row = ROW_ONE;
    int col = COL_ONE;


    int SuccesfulBind;
    //int SuccesfulRecv = FALSE; // not currently used

    SuccesfulBind = p_bind(2);

    if(SuccesfulBind){
        while(1){

            /*-----------------------------------------------------------------------------*/
            /* The following is an outline of the four arguments being received             */
            /* by this process from its queue. The four arguments are as follows            */
            /*                                                                              */
            /*      arg 1: the message queue from which to receive our message              */
            /*      arg 2: pointer to the location for the message source to be stored      */
            /*      arg 3: pointer to the location for the message to be stored             */
            /*      arg 4: the size of the message to be recieved                           */
            /*
            / *  note: the return value is TRUE if the msg was succesfully recvd            */
            /*        and FALSE if the msg was not succesfully recvd                        */
            /*------------------------------------------------------------------------------*/

            while(!(p_recv(2, &incomingMsgSrc, incomingMsg, 5))); // 5 because thats what the incoming message is


                uart_output_ch(row, col++, incomingMsg[0]);

                while(j < 100000){
                    j++;
                } j = 0;  // resets delay loop counter

                uart_output_ch(row, col++, incomingMsg[1]);

                while(j < 100000){
                    j++;
                } j = 0;  // resets delay loop counter

                uart_output_ch(row, col++, incomingMsg[2]);

                while(j < 100000){
                    j++;
                } j = 0;  // resets delay loop counter

                uart_output_ch(row, col++, incomingMsg[3]);

                while(j < 100000){
                    j++;
                } j = 0;  // resets delay loop counter

                uart_output_ch(row, col++, incomingMsg[4]);

                while(j < 100000){
                    j++;
                } j = 0;  // resets delay loop counter

                if(col > 80){ // if our text reaches 80 characters, return the column to 1 and increment the row
                    col = 1;
                    row++;
                }

            } // while(1) close bracket

    }
    else
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


       uart_output_ch(running[high_priority]->id, 1, strID[0]);
        i = 0; // resets delay loop counter
 //   }

    p_terminate(); // the function which will make the kernel command call

}

void proc4() // During development this process acts primarily as a never ending process which prints 1s
{
    int i = 0; // delay loop counter
    int SuccesfulBind;
    int SuccesfulSend;

    char * p1message = "abcde"; // character pointer to a location this proesses stack mem containing the message
    SuccesfulBind = p_bind(4); // SuccesfulBind will be TRUE if succesful bind occured, FALSE if bind failed

    /*-----------------------------------------------------------------------------*/
    /* The following is an outline of the four arguments being sent                 */
    /* by this process to another process. The four arguments are as follows        */
    /*                                                                              */
    /*      arg 1: destination of the message about to be sent                      */
    /*      arg 2: message source                                                   */
    /*      arg 3: a character pointer to somewhere in this processes stacks mem    */
    /*      arg 4: the size of the message to be sent                               */
    /*
     *  note: the return value is TRUE if the data was succesfully
     *        and FALSE if the data was not able to send the message                */
    /*------------------------------------------------------------------------------*/

    if(SuccesfulBind){
        while(1){

            while(i < 10000000){
                         i++;
            }

          SuccesfulSend = p_send(2, 4, p1message, 5); // arg 1: destination of the message about to be sent

          i = 0;
        }
    }
    else
      p_terminate();
}

void proc5() // During development this process acts primarily as a never ending process which prints 1 through 9 on
{

    int j = 0;// delay loop counter

    char exmpMsg1[1] = "a";
    char exmpMsg2[1] = "1";

    while(1){

        while(j < 1000000){
            j++;
        } j = 0;

         uart_output_ch(running[high_priority]->id, 1, exmpMsg1[0]); // 1 indicates the column number

         while(j < 1000000){
                 j++;
         } j = 0;

         uart_output_ch(running[high_priority]->id, 2, exmpMsg2[0]); // 2 indicates the column number

     }

}

void proc6() // During development this process acts primarily as a never ending process which prints 1s
{
    int row = running[high_priority]->id;
    int col = 1;
    char ch[1] = "1";

    int j = 0; // delay loop counter


    while(1){

        while(j < 1000000){
            j++;
        } j = 0;

        uart_output_ch(row, col++, ch[0]++);
        if (col > 80)
            col = 1;
        if (ch[0] > '9')
            ch[0] = '1';
    }

}

void proc7() // During development this process acts primarily as a never ending process which prints 1s
{
    int row = running[high_priority]->id;
    int col = 1;
    char ch[1] = "1";

    int j = 0; // delay loop counter

    while(1){

        while(j < 1000000){
            j++;
        } j = 0;

        uart_output_ch(row, col++, ch[0]++);
        if (col > 80)
            col = 1;
        if (ch[0] > '9')
            ch[0] = '1';
    }

}

void proc8() // During development this process acts primarily as a never ending process which prints 1s
{
    int row = running[high_priority]->id;
    int col = 1;
    char ch[1] = "1";

    int j = 0; // delay loop counter

    while(1){

        while(j < 1000000){
            j++;
        } j = 0;

        uart_output_ch(row, col++, ch[0]++);
        if (col > 80)
            col = 1;
        if (ch[0] > '9')
            ch[0] = '1';
    }

}

void proc9() // During development this process acts primarily as a never ending process which prints 1s
{

    int row = running[high_priority]->id;
    int col = 1;
    char ch[1] = "1";

    int j = 0; // delay loop counter
    int i = 0; // process duration loop counter

     while(i < 10){

         while(j < 1000000){
             j++;
         } j = 0;

         uart_output_ch(row, col++, ch[0]++);
         if (col > 80)
             col = 1;
         if (ch[0] > '9')
             ch[0] = '1';

         i++; // increments the terminating loop counter
     }

     p_terminate();

}

void proc10() // During development this process acts primarily as a never ending process which prints 1s
{
    int row = running[high_priority]->id;
    int col = 1;
    char ch[1] = "1";

    int j = 0; // delay loop counter
    int i = 0; // process duration loop counter

    while(1){
        while(i < 10){

             while(j < 1000000){
                 j++;
             } j = 0;

             uart_output_ch(row, col++, ch[0]++);
             if (col > 80)
                 col = 1;
             if (ch[0] > '9')
                 ch[0] = '1';

             i++; // increments the terminating loop counter
         }

        p_nice(2);

    }




}

void time_server() //
{

}

void monitor() //
{

    //int k = 0;// output position increment counter loop counter

    int column = COL_ONE;
    int row = ROW_ONE;

    int incomingMsgSrc; // memory location on this processeses stack for incoming message source
    char incomingMsg[1]; // an a pointer to an array of 5 characters. memory location on this processeses stack for incoming message


    while(1){
                    /*-----------------------------------------------------------------------------*/
                    /* The following is an outline of the four arguments being received             */
                    /* by this process from its queue. The four arguments are as follows            */
                    /*                                                                              */
                    /*      arg 1: the message queue from which to receive our message              */
                    /*      arg 2: pointer to the location for the message source to be stored      */
                    /*      arg 3: pointer to the location for the message to be stored             */
                    /*      arg 4: the size of the message to be recieved                           */
                    /*
                    / *  note: the return value is TRUE if the msg was succesfully recvd            */
                    /*        and FALSE if the msg was not succesfully recvd                        */
                    /*------------------------------------------------------------------------------*/

        while(!(p_recv(TO_MON, &incomingMsgSrc, incomingMsg, ONE_CHAR))); // we wait here until a message comes in from UART


        if(incomingMsg[0] == BACKSPACE){
            uart_output_ch(row, column--, incomingMsg[0]); // delete the previously input character from the monitor
        }
        else
            uart_output_ch(row, column++, incomingMsg[0]); // outputs the input character to the monitor
  //          uart_output_ch(row, column++, 'x'); // outputs the input character to the monitor

        if(column > 80){ // if our text reaches 80 characters, return the column to 1 and increment the row
            column = 1;
            row++;
        }

    } // end of while(1) container

}

void idle_proc() // currently is the get_id process
{
    int i = 1; // loading bar count variable
    int j = 0; // delay loop counter

    while(1){

        while(j < 100000){
                      j++;
        }

        if(i == 1){
            uart_output_ch(ROW_ONE, 79, '\\');
        }
        else if(i == 2){
            uart_output_ch(ROW_ONE, 79, '|');
        }
        else if(i == 3){
            uart_output_ch(ROW_ONE, 79, '/');
        }
        else if(i == 4){
            uart_output_ch(ROW_ONE, 79, '-');
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

/***************************************************************************************/

void pkcall(int code, void * structurePtr)
{

    struct pkargs pka;

    pka.code = code;
    pka.ptr_to_structure = structurePtr;

    p_assignR7((unsigned long) &pka);

    SVC();
}

int pkcall_with_return(int code, void * structurePtr)
{

    struct pkargs pka;

    pka.code = code;
    pka.ptr_to_structure = structurePtr;

    p_assignR7((unsigned long) &pka);

    SVC();

    return pka.rtnvalue;
}

/****************************************************************************************/

int p_get_id()
{
    volatile int pkRtn;

    pkRtn = pkcall_with_return(GETID, NULL); // NULL because getid doesnt have any arguments in a structure or address to point at

    return pkRtn;
}


void p_terminate()
{
    pkcall(TERMINATE, NULL); // NULL because terminate doesnt have any arguments in a structure or address to point at
}


void p_nice(int incr)
{
    volatile int incrVal = incr;
    pkcall(NICE, &incrVal);
}


int p_bind(int index) // return value used as success or failure flag
{
    volatile int pkRtn;

    volatile int indexVal = index;

    pkRtn = pkcall_with_return(BIND, &indexVal);

    return pkRtn;
}

int p_send(int to, int from, char * msg, int size){

    volatile struct pkSend  pk_send_args; /* Volatile to actually reserve space on stack*/

    int pkRtn; // TRUE or FALSE depending on success (TRUE) or failure (FALSE) of send

    pk_send_args.to = to;       // an integer value
    pk_send_args.from = from;   // an integer value
    pk_send_args.msg = msg;     // a character pointer to memory on the sending processes stack containing the message to send
    pk_send_args.size = size;   // an integer value

    pkRtn = pkcall_with_return(SEND, &pk_send_args); // this function facilitates the sending and receiving of messages accross pkspace

    return pkRtn; // TRUE or FALSE depending on success (TRUE) or failure (FALSE) of send

}

int p_recv(int my_mailbox, int * from, char * msg, int sz){

    volatile struct pkRecv  pk_recv_args; /* Volatile to actually reserve space on stack*/
   // send_msg('h', MONsrc, UARTq);
    int pkRtn; // TRUE or FALSE depending on success (TRUE) or failure (FALSE) of the receive call

    pk_recv_args.my_mailbox = my_mailbox; // an integer value
    pk_recv_args.from = from;             // a character pointer to memory on the sending processes stack containing the message to send
    pk_recv_args.msg = msg;
    pk_recv_args.size = sz;

    pkRtn = pkcall_with_return(RECV, &pk_recv_args);

    return pkRtn;
}

int uart_output_ch(int row, int col, char ch)
{


    /* Output a single character to specified screen position */
    /* CUP (Cursor position) command plus character to display */
    /* Assumes row and col are valid (1 through 24 and 1 through 80, respectively) */
    struct CUPch uart_data;
    /* Since globals aren’t permitted, this must be done each call */
    uart_data . esc = ESC;
    uart_data . sqrbrkt = '[';
    uart_data . line[0] = '0' + row / 10;
    uart_data . line[1] = '0' + row % 10;
    uart_data . semicolon = ';';
    uart_data . col[0] = '0' + col / 10;
    uart_data . col[1] = '0' + col % 10;
    uart_data . cmdchar = 'H';
    uart_data . ch = ch;

    return pkcall_with_return(UART_OUT_CH, &uart_data);
}



void p_assignR7(volatile unsigned long data)
{
    /* Assign 'data' to R7; since the first argument is R0, this is
     *simply a move from R0 to R7
     */

    __asm(" mov r7,r0");
}
