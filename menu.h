/*
 * menu.h
 *
 *  Created on: Oct 12, 2018
 *      Author: NickB
 */

#ifndef MENU_H_
#define MENU_H_

#define EMPTY_FLAG      -1
#define MAX_OUT_STR_LEN     17
#define MAX_BUFFER_SIZE     80
#define DISPLAYTBL_LEN1          63


#define TRUE    1
#define FALSE   0

#define NUL '\0'

#define UARTsrc    1
#define CLOCKsrc    2

#define MONq    0 // these are used to indicate what Q to enter
#define UARTq    1

#define MONsrc      3


#define BACKSPACE       0x7f
#define ESC             27

#define STARTFLAG       -1


void user_menu();

char str[MAX_BUFFER_SIZE]; // this is the buffer string holding the incoming characters from the uart once offloaded from the queue
char outStr[MAX_OUT_STR_LEN]; // this is the string that holds the output from the application layer

// this struct holds the command and the argument of the users input
struct userinput
{
    char * procName; // this is our first command
    char * priority; // this is our argument

};





char data; // memory locations sent to rec message by reference
int source;

struct userinput input; // this struct object is used in monitor.
// the return value of parser is 'input', and input will be sent to command finder to call the appropriate processes

/*this prototype is for the parse that tokenizes our command and argument, and attempts to identify it, the type is userinput*/
struct userinput parser(char Request[]);

void reg_proc(char * prc, int pri, int prID); // prototype for function that calls the appropraite application process

void show_reg_procs();


#endif /* MENU_H_ */
