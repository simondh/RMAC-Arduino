//
//  DNOCR_Mesg.cpp
//  DNOCR_LIB
//
//  Created by Simon Hewitt on 11/01/2015.
//  Copyright (c) 2015 Simon Hewitt. All rights reserved.
//

#include "DNOCR_Mesg.h"


DNOCR_Mesg::DNOCR_Mesg (const int qSize)
{
    int i;
    if (qSize > MAX_QSIZE) {
        _qSize = MAX_QSIZE;
        WHD_Util::writeLog(LOG_WARN, F("DNOCR_Mesg constructor qSize truncated from"), qSize);
    } else
        _qSize = qSize;
    
    for (i = 0; i < MAX_QSIZE; i++) Q[i] = NULL;  // set ALL to null first
    
    // now allocate those we are using
    for (i = 0; i < _qSize; i++)
    {
        Q[i] = (char *)calloc(MESGLEN, sizeof(char));
    }
    qHead = 0;
    qTail = -1;
}

int DNOCR_Mesg::enQueue (const char *mesg)
{
    // Adds message to the Q (if possible)
    // returns new Q-size or -1 on error
    
    if (qFull())
    {
        WHD_Util::writeLog(LOG_WARN, F("DNOCR_Mesg enQueue overlow, message lost"));
        return -1;  // cannot help
    }
    
    WHD_Util::safeCopy(Q[qHead], (char *)mesg, MESGLEN);
    if (qTail < 0) qTail = qHead;
    qHead++;
    qHead %= _qSize;
    
    return qSize();
}


int DNOCR_Mesg::deQueue ( char *mesg)
{
    // ditto
    *mesg = '\0';
    int qs = qSize();
    if (qTail < 0) return -1; // Queue is empty
    WHD_Util::safeCopy(mesg, Q[qTail], MESGLEN);
    if (qs == 1)
    {
        // now empty
        qTail = -1;
    } else {
        qTail++;
        qTail %= _qSize;
    }
    
    return qs-1;
}

int DNOCR_Mesg::qSize()
{
    // Returns the number of elements in the q, 0.._qSize
    
    if (qTail < 0) return 0;
    if (qHead == qTail) return _qSize;
    if (qHead >= qTail) return (qHead - qTail);
    else return (_qSize - qTail + qHead);
    
}

bool DNOCR_Mesg::qFull()
{
    return (qSize() >= _qSize);
}

