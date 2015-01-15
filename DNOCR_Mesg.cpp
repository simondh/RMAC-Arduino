//
//  DNOCR_Mesg.cpp
//  DNOCR_LIB
//
//  Created by Simon Hewitt on 11/01/2015.
//  Copyright (c) 2015 Simon Hewitt. All rights reserved.
//

#include "DNOCR_Mesg.h"
#include "DNOCR_Rmac.h"

std::string inputString = "";         // a string to hold incoming data
bool stringComplete = false;

#define CMD_SEG_LENGTH 40
#define MAX_PARAMS 20

/**
 ================  Constructors ====================
 **/

DNOCR_Mesg::DNOCR_Mesg (const int qSize, Site *thisSite)
{
    int i;
    if (qSize >= MAX_QSIZE) {
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
    siteObj = thisSite;  // We use tghis to check security amongst other things.
}

DNOCR_Mesg::~DNOCR_Mesg()
{
    // Free the char buffers calloc'ed in constrcutor
    for (int i = 0; i < _qSize; i++) {
        free(Q[i]);
    }
}

/**
 ================  Message receive and process methods====================
 **/

void dummyReadChar ()
{
    // simulating the effect of serialEvent
    // Reads a char if there is one, adds to inputString, sets stringComplete iof '\n'
    int c;
    
    return;
    
    if (!std::cin.eof())
        c = std::cin.peek();  // peek character

    if (c != EOF)
    {
        std::cin >> c;
        if ((c == '\n') || (c == '\r'))
        {
            stringComplete = true;
        } else
        {
            inputString += (char)c;
        }
    } // else nothing to do
}

void DNOCR_Mesg::checkMessages()
{
    // TODO: mostly gets built on Arduino
    // Check for any messages - SMS, or serial
    // Add to queue
    // (Main loop must call method to process them
    
    //http://arduino.cc/en/Tutorial/SerialEvent use serialEvent instead of this:
    dummyReadChar();
    if (stringComplete) {
        enQueue(inputString.c_str());
        inputString = "";
        stringComplete = false;
    }
    
    // TODO: process SMS here
    
}
 char *nextToken (char *mesgPtr, char *token)
{
    char *ptr = mesgPtr;
    while (*ptr && (*ptr <= ' ')) ptr++; // step over any leading blanks or non-printing
    char *pOut = token;
    int len = 0;
    
    while (*ptr && (*ptr != ' ') && (len < CMD_SEG_LENGTH)) {
        *(pOut++) = *(ptr++);
        len++;
    }
    *pOut = '\0';
    if (len >= CMD_SEG_LENGTH)
        WHD_Util::writeLog(LOG_WARN, "Message segment too long, truncated", mesgPtr);
    
    while (*ptr && (*ptr <= ' ')) ptr++; // step over any leading blanks or non-printing

    return ptr;
}

bool parseMessage (const char *thisMesg, char *msgPIN, char *msgCmd, char cmdArray[][CMD_SEG_LENGTH], const int max_params, int *actualParams)
{
    // thisMesg is the incoming message
    // format
    // ppppp cmd <parameters>
    // parse pppp, the pin, into msgPIN, cmd into msgCmd, up to max_params into cmdArray
    char *ptr;
    int i = 0;
    
    *actualParams = 0;
    
    ptr = nextToken((char *)thisMesg, msgPIN);
    if (msgPIN[0] == '\0') {
        // empty message!
        WHD_Util::writeLog(LOG_WARN, "Invalid message, no PIN", thisMesg);
        return false;
    }
    
    ptr = nextToken(ptr, msgCmd);
    if (msgCmd[0] == '\0') {
        // empty message!
        WHD_Util::writeLog(LOG_WARN, "Invalid message, no COMMAND", thisMesg);
        return false;
    }
    for (i = 0; i < strlen(thisMesg); i++) {
        msgCmd[i] = toupper(msgCmd[i]);
    }
    
    while ((i < max_params) && *ptr) {
        ptr = nextToken(ptr, cmdArray[i]);
        i++;
    }
    
    if (*ptr != '\0') {
        // have reached max_params but not reached the end of teh line, error
        WHD_Util::writeLog(LOG_WARN, "Message overflow, > MAX_PARAMS tokens", thisMesg);
    }
    
    *actualParams = i;
    
    return true;
}

bool processRequest (const char *thisMesg, char *msgPIN, char *msgCmd, char cmdArray[][CMD_SEG_LENGTH], int actualParams)
{
    // Its a Request command "Rxx"
    // RAL - all alarms
    // Rnn - alarms on channel nn
    
    
    
    return true;
}

bool processDisable (const char *thisMesg, char *msgPIN, char *msgCmd, char cmdArray[][CMD_SEG_LENGTH], int actualParams)
{
    // Its a Request command "Rxx"
    
    return true;
}

bool DNOCR_Mesg::processMessage()
{
    // Command messages are all of the form
    /*
     PPPPP CCC <parameters>
     where PPPPP is the 5 digit security PIN.
     CCC is the three letter command
     <parameters> are the specific parameters for the command
     */

    char msgPIN[CMD_SEG_LENGTH];
    char msgCmd[CMD_SEG_LENGTH];
    char cmdArray[MAX_PARAMS][CMD_SEG_LENGTH];
    bool ok;
    int actualParams;
    
    if (Queued() == 0) {
        return false;  // should not get here, but does not matter
    }
    
    char thisMesg[MESGLEN];
    deQueue(thisMesg);
    WHD_Util::writeLog(LOG_VERBOSE, "Processing message ", thisMesg);
    
    ok = parseMessage (thisMesg, msgPIN, msgCmd, cmdArray, MAX_PARAMS, &actualParams);
    
//    std::cout << "Mesg : " << thisMesg << std::endl;
//    std::cout << "Pin : " << msgPIN << " CMD : " <<msgCmd << std::endl;
//    for (int i = 0; i < actualParams; i++)
//        std::cout << "Params : " << i << " : " << cmdArray[i] << std::endl;
//    std::cout << "Actual params " << actualParams << std::endl;
    
    if (ok) {
        // process the message
        char commandType = msgCmd[0];
        
        switch (commandType) {
            case 'R':
                // Request command
                processRequest (thisMesg, msgPIN, msgCmd, cmdArray, actualParams);
                break;
            case 'D':
                // Disable command
                processDisable (thisMesg, msgPIN, msgCmd, cmdArray, actualParams);
                break;
                
                
            default:
                break;
        }
    } // else ignore it (log message already sent in parse.}
  
    
    return true;
}

/**
 ================  Message Queue methods ====================
 **/



int DNOCR_Mesg::enQueue (const char *mesg)
{
    // Adds message to the Q (if possible)
    // returns new Q-size or -1 on error
    
    if (qFull())
    {
        WHD_Util::writeLog(LOG_WARN, F("DNOCR_Mesg enQueue overlow, message lost"));
        return -1;  // cannot help
    }
    if (mesg[0] == '\0') {
        // dont queue an empty message!
        WHD_Util::writeLog(LOG_WARN, F("DNOCR_Mesg enQueue empty message, discarded!"));
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
    if ((qTail < 0) || (*Q[qTail] == 0)){
        return -1; // Queue is empty
    }
    WHD_Util::safeCopy(mesg, Q[qTail], MESGLEN);
    *Q[qTail] = 0;  // empty dequeued element
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
    // PROBLEM head == tail if Q is Full OR empty!
    
    if (qTail < 0) return 0;
    if (qHead == qTail)
    {
        if (*Q[qHead] == 0)
            return (0);
        else
            return _qSize;
    }
    if (qHead >= qTail) return (qHead - qTail);
    else return (_qSize - qTail + qHead);
    
}

bool DNOCR_Mesg::qFull()
{
    return (qSize() >= _qSize);
}

bool DNOCR_Mesg::Queued()
{
// True if any messages in Q

    return (qSize() != 0);
}

