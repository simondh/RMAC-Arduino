//
//  LFlash.h
//  DNOCR_LIB
//
//  Created by Simon Hewitt on 08/01/2015.
//  Copyright (c) 2015 Simon Hewitt. All rights reserved.
//

#ifndef __DNOCR_LIB__LFlash__
#define __DNOCR_LIB__LFlash__

#include <stdio.h>
#include <fstream>
#include <iostream>
#include <ctime>


#define FILE_READ 1
#define FILE_WRITE 2

// This definition of F() does nothing!
// However in native Arduino, it tells the compiler to keep the string in Flash program memory,
// so it does not use RAM on program load
#define F(string_literal) (string_literal)

class LFile
{
public:
    LFile(const char *f, int io);
    int read();
    int write (char c);
    int available();
    void close();
    void flush();
    
public:
    std::fstream f;
};


class LFlash
{
public:
    static bool exists (const char * fname);
    static void remove (const char *fname);
    static LFile open (const char *fname, int io);
    static  bool mkdir (const char * dirName);
    
    
};

/*
** DateTime
*/

typedef struct {
    int year;
    int mon;
    int day;
    int hour;
    int min;
    int sec;
} datetimeInfo;


class LDateTimeClass
{
public:
    
static int getTime(
            datetimeInfo * time
            );
    
static int getRtc(
           unsigned int * rtc
           );

static int setTime( // does nothing in this sim
            datetimeInfo * time
            );
    
};

class Serial
{
    // Emulation for Arduino Serial class
    // just writes to std::cout
    // for real - use the Arduino library!
public:
    
    static long print (const char * msg);
    static long println (const char * msg);
    static long print (int v );
    static long println (int v );
    static long print (float f);
    static long println (float f);
//    static long print (bool b);
//    static long println (bool b);
    
};



#endif /* defined(__DNOCR_LIB__LFlash__) */
