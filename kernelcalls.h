/*
 * kernelcalls.h
 *
 *  Created on: Oct 20, 2018
 *      Author: NickB
 */

#ifndef KERNELCALLS_H_
#define KERNELCALLS_H_

enum kernelcallcodes {GETID, NICE, TERMINATE, BIND, SEND, RECV};

struct kcallargs
{
    unsigned int code;
    unsigned int rtnvalue;
    unsigned int arg1; // to
    unsigned int arg2; // from
    char * arg3; // this will be used to hold the message from p_send
    unsigned int arg4; // size
    int * arg5;
};


#endif /* KERNELCALLS_H_ */
