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



#endif /* defined(__DNOCR_LIB__LFlash__) */
