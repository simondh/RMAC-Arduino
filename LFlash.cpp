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