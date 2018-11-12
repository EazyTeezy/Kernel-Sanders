/*
 * kernelcalls.h
 *
 *  Created on: Oct 20, 2018
 *      Author: NickB
 */

#ifndef KERNELCALLS_H_
#define KERNELCALLS_H_

typedef enum {GETID, NICE, TERMINATE, BIND, SEND, RECV, UART_OUT_CH} kernelcallcodes;

/*
 *          This structure is the shared memory between pspace and kspace
 */

struct pkargs
{
    int code;
    unsigned int rtnvalue;
    void * ptr_to_structure; // this contains the message callers specific parameters (e.g. send has to from msg sz)

};


/*
 *          The following structure are pointed to by an sub data type within the above structure
 */

struct pkSend // a pointer to an element of this type exists within pkargs
{
    int to;
    int from;
    char * msg;
    int size;
};

struct pkRecv // a pointer to an element of this type exists within pkargs
{

    int my_mailbox;
    int * from;
    char * msg;
    int size;
};

struct CUPch
{
    char esc;
    char sqrbrkt;
    char line[2];   /* 01 through 24 */
    char semicolon;
    char col[2];    /* 01 through 80 */
    char cmdchar;
    char ch;
};


#endif /* KERNELCALLS_H_ */
