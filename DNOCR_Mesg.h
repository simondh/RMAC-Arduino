//
//  DNOCR_Mesg.h
//  DNOCR_LIB
//
//  Created by Simon Hewitt on 11/01/2015.
//  Copyright (c) 2015 Simon Hewitt. All rights reserved.
//
//  This contains DNOCR_mesg class
//  Which maintains a queue of messages to be processed
//  and has methods to push, pop and process messages.
//
//  This is the main message processing class and will get huge.
//  All OTA commands are processsed here
//  And all outgoing messages.
//
//  allocator DNOCR_<esg(qsize) - max is MAX_QSIZE, 128
//  Dont create and destroy as Arduino garbage collection does not work well
//  i.e. allocate one, or two, and keep them (so at main level or static)


#ifndef __DNOCR_LIB__DNOCR_Mesg__
#define __DNOCR_LIB__DNOCR_Mesg__

#include <stdio.h>
#include "LFlash.h"
#include <ctype.h>
#include <ctime>
#include "DNOCR_Config.h"
#include "DNOCR_Rmac.h"
#include "WHD_Util.h"

// local constants etc

#define MAX_QSIZE 128
#define MESGLEN 170   // SMS is 160

class DNOCR_Mesg
{
    // This if a FIFO queue - first in (enQueue) first out (deQueue)
public:
    int enQueue (const char *mesg); // returns new Q-size or -1 on error
    int deQueue ( char *mesg); // ditto. mesg MUST be MESGLEN long
    int qSize();                // 0..qSize or -1 on error
    bool qFull();
    DNOCR_Mesg (const  int qSize);
    
private:
    char *Q[MAX_QSIZE]; // got that - its an array of char pointers.
    int qHead, qTail;
    int _qSize;
    
    
    
    
};


#endif /* defined(__DNOCR_LIB__DNOCR_Mesg__) */
