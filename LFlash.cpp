//
//  LFlash.cpp
//  DNOCR_LIB
//
//  Created by Simon Hewitt on 08/01/2015.
//  Copyright (c) 2015 Simon Hewitt. All rights reserved.
//

#include "LFlash.h"
#include "stdio.h"
#include <sys/param.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

/**
 **=================  LFile ===================
 **/


LFile::LFile(const char *fn, int io)
{
    if (io == 2)
        f.open(fn, std::ifstream::out );
    else if (io == 3)
        f.open(fn, std::ifstream::out | std::ifstream::app );
    else
        f.open(fn, std::ifstream::in );

    //char cCurrentPath[FILENAME_MAX];
    
    //    getcwd(cCurrentPath, sizeof(cCurrentPath));
    //    std::cout << cCurrentPath << std::endl;
    
}


int LFile::read()
{
    int c;
    c = f.get();
    return (c);
}

int LFile::write (char c)
{
    f.put(c);
    f.flush();
    
    return 1;
}

int LFile::available()
{
    return (!f.eof());
}

void LFile::close()
{
    f.flush();
    f.close();
}

void LFile::flush()
{
    // does nothing
}

/**
 **=================  LFlash ===================
 **/

bool LFlash::mkdir (const char * dirName)
{
    int status;

    status = ::mkdir("LOGS", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    return status == 0;
}



bool LFlash::exists (const char * fname)
{
    std::ifstream f(fname);
    if (f) {
        f.close();
        return true;
    } else
        return false;
}

void LFlash::remove (const char *fname)
{
    std::remove(fname);
}

LFile LFlash::open (const char *fname, int io)
{
    if (io == 2)
    {
        io = io | std::ios_base::app;
    }
    LFile f(fname, io);
    
    return f;
}

/**
 **=================  LDateTimeClass ===================
 **/

int LDateTimeClass::getTime(datetimeInfo * theDateTime)
{
    // datetimeinfo is a struct , defined in LFlash.h
    // Returns dat-time values in datetimeInfo struct
    // same as LTime LinkIt lib

    time_t t = time(0);   // get time now
    struct tm * now = localtime( & t );
    theDateTime->year = now->tm_year + 1900;
    theDateTime->mon = now->tm_mon;
    theDateTime->hour = now->tm_hour;
    theDateTime->day = now->tm_mday;
    theDateTime->min = now->tm_min;
    theDateTime->sec = now->tm_sec;
 
    // if succeed, return 0, otherwise return -1
    return 0;
}

int LDateTimeClass::getRtc(unsigned int * rtc)
{
    // get real-time in seconds sine 1/1/1970, into &rtc
    time_t  timev;
    time(&timev);  // returns time since 1/1/1970
    *rtc = (unsigned int) timev;
    return 0;
}

int LDateTimeClass::setTime( datetimeInfo * time )
{
    // In this OS X sim, this does nothing (as time set anyway)
    return 0;
}


/**
 **=================  Serial ===================
 **/


long Serial::print (const char * msg)
{
    std::cout << msg;
    return (strlen(msg));
}

 long Serial::println (const char * msg)
{
    std::cout << msg << std::endl;
    return (strlen(msg));
}

 long Serial::print (int v )
{
    char buff[32];
    sprintf (buff, "%d", v);
    return Serial::print(buff);
}

 long Serial::println (int v )
{
    return(Serial::print(v));
}

 long Serial::print (float f)
{
    char buff[32];
    sprintf (buff, "%f", f );
    return Serial::print(buff);
}
 long Serial::println (float f)
{
    return Serial::print(f);
}

/*
long Serial::print (bool b)
{
    if (b) {
        std::cout << "true" << std::endl;
        return 4;
    }
    else {
        std::cout << "false" << std::endl;
        return 5;
    }
}

 long Serial::println (bool b)
{
    if (b) {
        std::cout << "true" << std::endl;
        return 4;
    }
    else {
        std::cout << "false" << std::endl;
        return 5;
    }
}
*/


