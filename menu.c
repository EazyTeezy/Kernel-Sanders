/*
 * menu.c
 *
 *  Created on: Oct 12, 2018
 *      Author: NickB
 */

#include "menu.h"
#include "MSGpasser.h"
#include "ISRs.h"
#include "kernel.h"
#include "procs.h"

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>




void user_menu()
{
    int i = 0; // indexes the string we're building to send to the parser
    int escapeCnt = 0; // used when ignoring escape keys
    send_msg('>', MONsrc, UARTq);
    int startFlag = FALSE;

    while(startFlag == FALSE){
      //  send_msg('.', MONsrc, UARTq);
        if (rec_msg(&data, &source, MONq)){ // this means 'if a message exists in the to monitor queue / the queue is not empty' b/c rec_msg returns a T/F flag

                send_msg(data, MONsrc, UARTq); // sends the users input key back to the screen via UART

                if(data == ESC || escapeCnt != 0 ){ // if an escape key is detected, the following two keys are ignored
                    escapeCnt++;
                    if(escapeCnt == 3)
                        escapeCnt = 0;
                }
                else if(data == '\r'){ // if the enter key was pressed
                    i=0; // resets our buffer string
                    input = parser(str); // at this point  the buffer command string is parsed

                    if(strcmp(input.procName, "START") == 0) // checks to see if the user sent in the start command
                        startFlag = TRUE;
                    else if(strcmp(input.procName, "DISPLAY") == 0) // checks to see if the user sent in the start command
                        show_reg_procs();// DISPLAY processes;
                    else{
                 //       reg_proc(input.procName, input.priority); // this function registers our requested processes      // commented this out because 'too few arguments in fn call'
                        send_msg('>', MONsrc, UARTq);
                    }
                }
                else if(data == BACKSPACE){ // check for backspace
                    str[--i] = '\0'; // if backspace detected, last entered character is removed from buffer string
                }
                else{
                    str[i++] = data; // places the new char in our buffer command string
                    str[i] = '\0';
                }
        }
    }

}

/* what will this functions
 * return value be fill in here */


/* this function parses through the incoming data separating it by delimiters (spaces) */
struct userinput parser(char Request[]){

    char * procNum; // this is our users entered proccess name
    char * priorityVal; // this is the provided priority
    char * shouldntExist; // used in error cases
    char UPcmd[MAX_BUFFER_SIZE]; // uppercase command
    int i = 0; // used to convert entered commands to uppercase

    procNum = strtok(Request, " ");
    if(procNum != NULL){
        priorityVal = strtok(NULL, " ");
    }
    if(priorityVal != NULL){
        shouldntExist = strtok(NULL, " ");
        if(shouldntExist != NULL)
           ;// this is an error case, will probably have to send a '?' to the uart here *********************
    }
       /* converts entered commands to upper case*/
    while (i < strlen(procNum)){
         UPcmd[i] = toupper(procNum[i]);
         i++;
    } i = 0;

    while(i < strlen(procNum)){
        procNum[i] = UPcmd[i];
         i++;
    }

    struct userinput UIrtrn = {procNum, priorityVal};

    return UIrtrn;
}




void show_reg_procs(){

    int i = 0; // facilitates sending the output string
    char displayStr[DISPLAYTBL_LEN1] = "\n\nProccess Name:      ID:         Allocated Stack Range:\n\n";

        while(i < strlen(displayStr)){
            send_msg(displayStr[i], MONsrc, UARTq); // sends message to the UART q from the MON src
            i++;
        }
        send_msg('>', MONsrc, UARTq); // gives out command promt symbol

}
