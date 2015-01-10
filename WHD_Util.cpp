//
//  WHD_Util.cpp
//  DNOCR_LIB
//
//  Created by Simon Hewitt on 07/01/2015.
//  Copyright (c) 2015 Simon Hewitt. All rights reserved.
//

#include <cstring>
#include <ctype.h>
#include <ctime>
#include "DNOCR_Config.h"
#include "WHD_Util.h"

#define CRLF "\r\n"  // Windows line termination

/*
  ** WHD_Util - mostly staic utility routines
*/


int WHD_Util::safeCopy (char *dest, char *source, int l)
{
    // fussy, but make sure a) Name cannot overflow, and b) is null terminated
    memset(dest, 0, l );
    strncpy (dest, source, l-1);
    return (int)(strlen(dest));
}

 int WHD_Util::cleanTelephone (char *tnum)
{
    char *p, *opptr, *opEnd;
    char op[MaxPhoneLength];
    
    WHD_Util::cleanStr(tnum);

    
    p = tnum;
    memset(op, 0, MaxPhoneLength);
    opptr = op;
    opEnd = opptr + MaxPhoneLength - 1;
    
    // convert + to 00 if present
    if (*p == '+'){
        p++;
        op[0] = '0'; op[1] = '0';
        opptr += 2;
    } else  if ((*p == '0') && (*(p+1) != '0')) {
        // we have a national number 01672.....
        // add 0044
        strcpy(op, "0044");
        opptr += 4;
        p++;
    }
    
    while (*p) {
        if (isdigit(*p))
            *(opptr++) = *p;
        p++;
        if (opptr >= opEnd) break; // too long, truncate
    }
    *opptr = '\0';
    
    strcpy(tnum, op);
    return ((int)strlen(tnum));
}

void WHD_Util::cleanStr(char *s)
{
    // removes leading and trailing spaces, multiple spaces reduced to one, removes any nonprinting chars
    // MUST be null terminated.
    // edits inplace (as output cannot be longer than input
    
    char *iptr, *optr;
    int wasSpace = 0;
    
    optr = iptr = s;
    
    // skip leading spaces
    while (*iptr == ' ') iptr++;
    
    // ignore non printing, strip multiplke spaces.
    while (*iptr) {
        if (*iptr >= 32){
            if (*iptr == ' '){
                if (!wasSpace) {
                    wasSpace = 1;
                    *optr = *iptr;
                    optr++;
                }
            }
                else {
                    wasSpace = 0;
                    *optr = *iptr;
                    optr++;
                }
        }
        iptr++;
    }
    
    *optr = '\0';
    optr--;
    
    // remove any trailing spaces
    while ((*optr == ' ') && (optr >= s))
    { *(optr--) = '\0'; }
    
}

void WHD_Util::cleanNumStr (char *s)
{
    char *ip, *op;
    int foundDecimal = 0;
    
    WHD_Util::cleanStr(s);
    ip = op = s;
    if ((*ip == '+') || (*ip == '-')) {
        ip++; op++;
    }
    
    while (*ip) {
        if ((isdigit(*ip)) || (*ip == '.')) {
            if (*ip == '.'){
                if (!foundDecimal) {
                    foundDecimal = 1;
                } else {
                    break; // 2nd '.', end it here!
                }
            }
            *(op++) = *ip;
        }
        ip++;
    }
    if (*(op-1) == '.') {*(op++) = '0';}  // avoid pattern "22."
    *op = '\0';
    
    // avoid stupid patterns like "+", "+."
    if ((strlen(s) < 2) && ((s[0] == '+') || (s[0] == '-') || (s[0] == '.'))){
        s[1] = '0'; s[2] = '\0';
    }
}




int WHD_Util::dayOfEpoch()
{
    // return number of days since 1 Jan 2015
    time_t  timev;
    time(&timev);  // returns time since 1/1/1970
    
    return (int)(timev/(24*60*60) - 45*365 - 10);
    
}

void writeLogString (LFile *f, const char *line)
{
    const char *p;
    
    p = line;
    while (*p) {
        f->write((int)*p);
        p++;
    }
    f->flush();
}


 void WHD_Util::writeLog (char logType, const char *s)
{
// /log/RMAC000001.LOG etc
    
    // slightly complex auto-create here
    // file is created on first use and on change of day
    // and /log dir created if necessary
    
    int i = dayOfEpoch();
    char logTypeStr[12];
    
    // first check /logs exists
    if (!LFlash::exists("LOGS")) {
        LFlash::mkdir("LOGS");
    }
    
    char fn [10];
    sprintf (fn, "LOGS/RMAC%06d.LOG", i );
    LFile f = LFlash::open(fn, FILE_WRITE);  // TODO check opens as append (LinkIt should, C++ does not)
    // TODO put date time here
    switch (logType) {
        case 'I':
            strcpy(logTypeStr, " Info ");
            break;
        case 'W':
            strcpy(logTypeStr, " Warn ");
            break;
        case 'E':
            strcpy(logTypeStr, " Error ");
            break;
            
        default:
            strcpy(logTypeStr, " internal error unknown log type ");
            break;
    }
    
    // TODO write time and date here
    writeLogString(&f, logTypeStr );
    writeLogString(&f, s );
    writeLogString(&f, CRLF );
    
    // TODO local debug
    std::cout << logTypeStr << " : " << s << std::endl;

    f.close();
}

 void WHD_Util::fatalError (const char * err)
{
// if program really cannot progress - just sits there flashing LEDs!
    writeLog(LOG_ERROR, "Fatal error, program cannot continue");
    writeLog(LOG_ERROR,err );
    // TODO flash blue and red LEDs continually
    
    exit(99);
    
}


