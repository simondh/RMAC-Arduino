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
#include "LFlash.h"
#include <iostream>
#include <ctime>
#include <stdio.h>
#include <string.h>
#include <limits.h>

#define CRLF "\r\n"  // Windows line termination
#define BUF_SIZE 256


/*
  ** WHD_Util - mostly staic utility routines
*/

/*
 ** ========================== String manipulation & cleaning ===============
*/
#pragma mark String manipulation & cleaning


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


/*
 ** ========================== date & Time ===============
 */
#pragma mark Date and Time

int ardTime::dayOfEpoch()
{
    // return number of days since 1 Jan 2015
    // STATIC method i.e. needs no object, call as ardTime::dayOfEpoch();
    unsigned int rtc;
    
    LDateTimeClass::getRtc(&rtc); // returns time since 1/1/1970
    
    return (int)(rtc/(24*60*60) - 45*365 - 10); // return days since 1 Jan 2015
}




ardTime::ardTime( bool now)
{
    // Do I need to malloc? - NO!, so no need for a destructor either
    if (now)
    {
        this->setArdTimeNow();
    }else {
        memset(&this->_dt, 0, sizeof(this->_dt));
    }
}

ardTime::ardTime()
{
    memset(&this->_dt, 0, sizeof(this->_dt));
   
}

ardTime::ardTime(char *timeStr)
{
    // Expect a timestamp in the form
    // TODO: Got here !!
    
    setArdTimeFromStr(timeStr);

}


 void ardTime::getArdTimeStr ( char *dt, const char *seperators)
{
    // theDate is a date struct with the time to be printed
    // 3-char separators string gives :
    // (if s[0] is A, s[1] is B, s[2] is C -> yyyyAmmAddBhhCmmCss
    // eg (datebuff, "/-:") -> yyyy/mm/dd-hh:mm:ss
    
sprintf (dt, "%04d%c%02d%c%02d%c%02d%c%02d%c%02d ",
         _dt.year, seperators[0], _dt.mon+1, seperators[0], _dt.day,
         seperators[1],
         _dt.hour, seperators[2], _dt.min, seperators[2], _dt.sec);
}

void ardTime::getArdTimeStr (char *str)
{
    // buff must be 22 chars. Returns yyyy-mm-dd hh:mm:ss
    getArdTimeStr(str, (char *)"- :");
}

void ardTime::setArdTimeNow()
{
    LDateTimeClass::getTime(&_dt);
}

void ardTime::setArdTimeZero()
{
    memset(&this->_dt, 0, sizeof(this->_dt));
}
bool ardTime::setArdTimeFromStr(const char *tmStr)
{
    sscanf (tmStr, "%d-%d-%d-%d-%d-%d", &this->_dt.year, &this->_dt.mon, &this->_dt.day,
                &this->_dt.hour, &this->_dt.min, &this->_dt.sec);
    return true;
}

long ardTime::secondsDifference()
{
    ardTime *now = new ardTime();
    return secondsDifference(now);
}


long ardTime::secondsDifference(const ardTime *secondDate)
{
    // returns seconds by which second date is after first date
    // negative if second date is BEFORE first date
    // TODO: Depends on std libary, may not work under Arduino.
    
    struct std::tm a ;
    struct std::tm b ;
    
    a.tm_year= _dt.year;
    a.tm_mon = _dt.mon;
    a.tm_mday = _dt.day;
    a.tm_hour = _dt.hour;
    a.tm_min = _dt.min;
    a.tm_sec = _dt.sec;
    a.tm_gmtoff = 0;
    a.tm_isdst = 0;
    b.tm_year= secondDate->_dt.year;
    b.tm_mon = secondDate->_dt.mon;
    b.tm_mday = secondDate->_dt.day;
    b.tm_hour = secondDate->_dt.hour;
    b.tm_min = secondDate->_dt.min;
    b.tm_sec = secondDate->_dt.sec;
    b.tm_gmtoff = 0;
    b.tm_isdst = 0;
    
    std::time_t x = std::mktime(&a);
    std::time_t y = std::mktime(&b);
    if ( x != (std::time_t)(-1) && y != (std::time_t)(-1) )
    {
        double difference = std::difftime(y, x);
        return  difference ;
    }
    return 0;
}

long   ardTime::minutesDifference(const ardTime *startDate)
{
    datetimeInfo now;
    LDateTimeClass::getTime(&now);
    
    return this->secondsDifference(startDate)/60;
}

 unsigned long  WHD_Util::millisSince (unsigned long now, unsigned long then)
{
    // only reason this is here is that the Arduino Millis function overflows back to zero every 50 days
    // Normally now > then, but after overflow, then > now, by a big margin
    // makes a judgement - difference up to 10 days is a negative time, else its overflow.
    // For longer time periods, use the date methods above.
    
    if (then > now) {
        // has overflowed
        return (ULONG_MAX - then + now -1);
    } else
        return (now - then);
}
 unsigned long WHD_Util::millisSince (unsigned long then)
{
    return (millisSince(millis(), then));
}


/*
 ** ========================== Log file ===============
 */
#pragma mark Log File routines

void writeLogString (LFile *f, const char *line)
{
    const char *p;
    //Also write to serial (which is discarded unless a serial minotor is attached, part of the Arduino IDE
    // And using my LFlash.h /.cpp, Serial:print reroutes to std::cout
    
    Serial::print(line);
    
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
    
    int i = ardTime::dayOfEpoch();
    char logTypeStr[12];
    char dateTimeStr[30];
    bool verbose = VERBOSE_LOG_ON;
    ardTime *tm = new ardTime(false);  // init, do not set to now.
    // first check /logs exists
    if (!LFlash::exists("LOGS")) {
        LFlash::mkdir("LOGS");
    }
    
    char fn [32];
    sprintf (fn, "LOGS/RMAC%06d.LOG", i );
    LFile f = LFlash::open(fn, FILE_WRITE);  // TODO: check opens as append (LinkIt should, C++ does not)

    switch (logType) {
        case 'A':
            strcpy(logTypeStr, " ALARM "); break;
        case 'I':
            strcpy(logTypeStr, " Info "); break;
        case 'W':
            strcpy(logTypeStr, " Warn "); break;
        case 'E':
            strcpy(logTypeStr, " Error ");break;
        case 'V' :
            strcpy(logTypeStr, " Verbose ");
            if (!verbose ) {
                return;
            }
            break;

        default:
            strcpy(logTypeStr, " internal error unknown log type "); break;
    }
    
    
    tm->getArdTimeStr(dateTimeStr);
    writeLogString(&f, dateTimeStr );
    writeLogString(&f, logTypeStr );
    writeLogString(&f, s );
    writeLogString(&f, CRLF );

    f.close();
}

void WHD_Util::writeLog (char logType, const char *s, const int n)
{
    char buff[BUF_SIZE];
    sprintf(buff, "%s : %d", s, n);
    writeLog(logType, buff);
}

void WHD_Util::writeLog (char logType, const char *s, const char *s2)
{
    char buff[BUF_SIZE];
    sprintf(buff, "%s : %s", s, s2);
    writeLog(logType, buff);
}



/*
 ** ========================== Error handling ===============
 */
#pragma mark Error handling



 void WHD_Util::fatalError (const char * err)
{
// if program really cannot progress - just sits there flashing LEDs!
    writeLog(LOG_ERROR, "Fatal error, program cannot continue");
    writeLog(LOG_ERROR,err );
    // TODO: flash blue and red LEDs continually
    
    exit(99);
    
}


void WHD_Util::fatalError (const char * err, const char * attr)
{
    // two strings such as fatalError ("Invalid Parameter", "siteName");
    char buff[BUF_SIZE], *p;
    p = (char *)attr;
    if (*p == '&') p++;  // some attributes are &attribute, ignore this
    sprintf (buff, "%s : %s", err, p);
    WHD_Util::fatalError(buff);
}

void WHD_Util::fatalError (const char * err, const int attr)
{
    char buff[BUF_SIZE];
    sprintf (buff, "%s : %d", err, attr);
    WHD_Util::fatalError(buff);
}



