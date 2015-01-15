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
//  It reads messages from SMS, serial interface (or anything else coming in)
//  Messages can arrive complete (SMS) or
//  be built up char by char (serial).
//  They are added to the Q when complete
//
//  Main loop is expected to call the processing side
//
//  Message processing - check, decompose and buld a command
//  and call an execution class (TBD!)


//  This is the main message processing class and will get huge.
//  All OTA commands are processsed here
//  And all outgoing messages.
//
//  allocator DNOCR_mesg(qsize) - max is MAX_QSIZE, 128
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
    DNOCR_Mesg (const  int qSize,  Site *thisSite);
    ~DNOCR_Mesg();
    

    int enQueue (const char *mesg);
    // returns new Q-size or -1 on error
    int deQueue ( char *mesg); // ditto. mesg MUST be MESGLEN long
    int qSize();                // 0..qSize or -1 on error
    bool qFull();
    bool Queued();      // True if any messages in Q
    void checkMessages();   // called from main lopp to check for any messages
    bool processMessage();  // Read, parse and deal with!
private:
    char *Q[MAX_QSIZE]; // got that - its an array of char pointers.
    int qHead, qTail;
    int _qSize;  // Capacity of Q
    Site *siteObj;   // The main site object being used.
    
    
    
};


#endif /* defined(__DNOCR_LIB__DNOCR_Mesg__) */
