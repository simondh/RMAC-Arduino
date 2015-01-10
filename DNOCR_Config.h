//
//  DNOCR_Config.h
//
//  Created by Simon Hewitt on 08/01/2015.
//  Copyright (c) 2015 Simon Hewitt. All rights reserved.
//
//  Utility classes for object serialisation/deserialisation, and managing the config files in LinkIt flash memory
//  Flash is non-volatile, so it is necessary to save and reload the configuration data over a power cycle.
//

#ifndef __DNOCR_Config__
#define __DNOCR_Config__

#include <stdio.h>
#include "WHD_Util.h"
#include "LFlash.h"



class DNOCR_Config
{
    // This class manages the config files
    // config files are stored in flash memory, a current and a single backup file, see CONFIG defines
    // Files are ASCII name:value pairs, one per line, Windows CR LF line-ending
    // Note the LFlash class is pretty basic, not even rename, and all bhyte-by-byte read/write
    // so much of this class just delivers line read/write, rename etc
    // Every class that needs save/restore must contain a saveClass and restoreClass method like:
    // which must write:
    //  class:className
    //  elementname:<data>
    //  classEnd:className
    //
    // The save and restore is implemented from top level functions that marshall the number and order of classes written
    // will work out version control later!
    
public:

    DNOCR_Config ();        // constructor
    void startSave();       // creates backup, opens new file, writes header
    int writeSaveLine (const char *line);
    int writeSaveString (const char *line);
    // convenience methods to write the Name Value Pair eg writeNVP ("SiteID", 99);
    int writeNVP (const char * name, const char *v);
    int writeNVP (const char * name, const int v);
    int writeNVP (const char * name, const float v);
    int writeNVP (const char * name, const bool v);
    int closeSave();
    
    
    int startRestoreConfig();
    bool readNVPCheck (const char *name, const char *value);  // returns error if file NVP does not match name:value
    bool readNVPString (const char *name, char *val, int valLength);  // returns the value as a string
    bool readNVPInt (const char *name, int *val);  // returns the value as a int
    bool readNVPBool (const char *name, bool *val); // bool
    int closeRecover();
    
private:
    static void renameFile (const char *newFile, const char * oldFile); // actually copies byte by byte
    static void makeBackup();    // calls renameFile() to move .dat to .bck

    LFile *saveFile;
    LFile *loadFile;
};

#endif /* defined(__DNOCR_Config__) */
