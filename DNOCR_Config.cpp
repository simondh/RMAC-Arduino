//
//  DNOCR_Config.cpp
//
//  Created by Simon Hewitt on 08/01/2015.
//  Copyright (c) 2015 Simon Hewitt. All rights reserved.
//

#include "DNOCR_Config.h"
#include "LFlash.h"
/*
 ** DNOCR_Config - Methods to manage the save and restore of classes from a Flash config file
 ** The LinkitOne has 10MB of Flash memory that is available through the LFlash class.
 ** Obviously the RAM memory is lost if the power is turned off, so any configuration revceived OTA (Over The Air)
 ** will also be lost.
 ** To fix this issue, this class enables the configuration classes to a) Read their initial state from a configuration file
 ** b) Write their state to a configuration file.
 ** Three files are used RMAC.CFG is the factory configuration file, is read but never changed. 
 **    If not present, a blank unit is set up
 ** RMAC.DAT is the curent configuration
 ** RMAC.BAK is the previous config, only one generation is kept.
 ** On startup, if RMAC.DAT exists, it is read and used. If not, if RMAC.CFG exists, it is read, and immediately saved to CFG.DAT
*/

#define CRLF "\r\n" // Windows line endings!
#define BUFFSIZE 128

// forward decalarations
int readUnarchiveLine (char *line, LFile *f);


DNOCR_Config::DNOCR_Config()
{
    // Constructor, does nothing
}

void DNOCR_Config::renameFile (const char *newFile, const char * oldFile)
{
    // "Rename" oldFile to newFile
    // As LinkIt LFlash does not have rename, actually laboriously creates a new file and copies byte by byte

    int c;
    
    if (LFlash::exists(newFile)) {
        LFlash::remove(newFile);
    }
    
    if (LFlash::exists(oldFile)){
        
        LFile ofile = LFlash::open(oldFile, FILE_READ);
        LFile nfile = LFlash::open(newFile, FILE_WRITE);
        
        while (ofile.available()) {
            c = ofile.read();
            if (c > 0)
                nfile.write(c);
        }
        
        ofile.close();
        nfile.close();
    }
}


void DNOCR_Config::makeBackup()
{
// calls renameFile() to move .dat to .bck
    DNOCR_Config::renameFile(CONFIGFILENAME CONFIGBACKUPEXT, CONFIGFILENAME CONFIGFILEEXT);
}

void DNOCR_Config::startSave()
{
// creates backup, opens new file, writes header
   // DNOCR_Config::makeBackup();
   // LFile f((char *)(CONFIGFILENAME CONFIGFILEEXT));
    
    this->makeBackup();
    saveFile = new LFile((char *)(CONFIGFILENAME CONFIGFILEEXT), 2);

    int i;
    
    i = this->writeSaveLine("Copyright DNOC 2015");
    i = this->writeSaveLine("RMAC configuration file");
}

/**
 ** SAVE methods
 **/


int DNOCR_Config::writeSaveString (const char *line)
{
    int i = 0;
    const char *p;
    
    p = line;
    while (*p) {
        saveFile->write((int)*p);
        i++; p++;
    }
    return i;
}


int DNOCR_Config::writeSaveLine (const char *line)
{
    // Write the string followed by CR LF
    int lLength = 0;
    
    lLength = this->writeSaveString (line);
    lLength += this->writeSaveString ((char *)(CRLF));
    
    return lLength;
}

// the folowing are convenience methids to write the Name Value pairs
int DNOCR_Config::writeNVP (const char * name, const char *v)
{
    int lLength = 0;
    lLength = writeSaveString(name);
    lLength += writeSaveString(":");
    lLength += writeSaveString(v);
    lLength += writeSaveString(CRLF);
    return lLength;
}

int DNOCR_Config::writeNVP (const char * name, const long n)
{
    char buff[BUFFSIZE];
    snprintf(buff, BUFFSIZE, "%ld", n);
    return (writeNVP(name, buff));
}

int DNOCR_Config::writeNVP (const char * name, const float f)
{
    char buff[BUFFSIZE];
    snprintf(buff, BUFFSIZE, "%f", f);
    return (writeNVP(name, buff));
}

int DNOCR_Config::writeNVP (const char * name, const bool b)
{
    if (b)
        return (writeNVP(name, "true"));
    else
        return (writeNVP(name, "false"));

}

int DNOCR_Config::writeNVP (const char * name, const datetimeInfo &dt)
{
    char buff [64];
    WHD_Util::timeStamp(buff, (char *)"---", &dt);  // TODO: check dont trust &
    return writeNVP(name,  buff);
    return 1;
}


int DNOCR_Config::closeSave()
{
    saveFile->close();
    return 1;
    
}

/**
 ** RESTORE methods
 **/


int DNOCR_Config::startRestoreConfig()
{
    char fn[BUFFSIZE];
    char target[] = "Copyright DNOC";
    char target2[] = "RMAC";
    
    WHD_Util::writeLog(LOG_INFO, "startRestoreConfig starting");
    
    // open RMAC.DAT if it exists
    strcpy(fn, CONFIGFILENAME CONFIGFILEEXT);
    if (LFlash::exists(fn)) {
            loadFile = new LFile((char *)(fn), 1);
    } else
    {
        // else open RMAC.CFG
        strcpy(fn, CONFIGFILENAME CONFIGDEFAULTEXT);
        if (LFlash::exists(fn)) {
            loadFile = new LFile((char *)(fn), 1);
        }
        else {
            WHD_Util::fatalError("No configuration file");
        }
    }

    WHD_Util::writeLog(LOG_INFO, "startRestoreConfig file open");
    
    // First read and discard two lines of file that are copyright and title
    readUnarchiveLine(fn, loadFile);
    if (strncmp(fn, target, strlen(target)) != 0) WHD_Util::fatalError("Config file line 1 is not Copyright DNOC");
    readUnarchiveLine(fn, loadFile);
    if (strncmp(fn, target2, strlen(target2)) != 0) WHD_Util::fatalError("Config file line 2 is not RMAC");

    return 1;
}


int readRawUnarchiveLine (char *line, LFile *f)
{
    // Reads a whole line from unarchive file
    // skipping blank lines and comment lines
    // comments start with / (but use // for style)
    // returns line length or -1 for EOF
    // DANGER asumes bugffer is 128 (BUFFSIZE) bytes!
    
    
    char *p, c;
    int i = 0;
    bool isComment = false;
    
    p = line;
    c = f->f.get();  //TODO: will have to change
    
    while ((i < BUFFSIZE) && (c != '\n') && (c != '\r')) {
        if (c < 0) {
            // EOF
            return -1;
        }
        if ((i == 0) && (c == '/')) {
            // comment, return 0
            isComment = true; // read rest of line but discard it
        }
        *p = c;
        p++; i++;
        c = f->f.get();  //TODO: will have to change
    }
    
    *p = '\0';
    c = f->f.peek();
    if (c < ' ') {
        f->f.get(); // CRLF, read & discard last one
    }
    
    WHD_Util::cleanStr(line);
    
    i = isComment?0:(int)strlen(line); // ensure comment lines are discarded
    
    return i;
}

int readUnarchiveLine (char *line, LFile *f)
{
    int r;
    
    while ((r = readRawUnarchiveLine(line, f)) == 0) {
        // do nothing
    }
    return r;  // at this point  either EOF or valid line - blanks and comments discarded.
}


bool parseUnarchiveLine (char *name, char *val, int valSize, LFile *f)
{
    // NVP should be <name:a value here>
    // puts name in name, value in value, returns false on error
    // name and val must point to char arrays big enough to receive:
    // name (usually short-ish, say 32), and value, could be up to 100
    // and overall line must not exceed BUFFSIZE, 128
    
    char buff[BUFFSIZE];
    char *p, *n;
    name[0] = '\0';
    val[0] = '\0';
    
    readUnarchiveLine(buff , f);
    p = buff;
    n = name;
    while ((*p) && (*p != ':')) {
        *(n++) = *(p++);
    }
    *n = '\0';
    WHD_Util::cleanStr(name);
    if (*p != ':'){
        return false; // no ':' found
    } else {
        p++; // skip ':'
    }
    
    if ((strlen(name) == 0)|| (*p == '\0')) {
        // either no name part or reached end of line with no value
        return false;
    }
    
    n = val;
    while ((*p) && (valSize--)) {
        *(n++) = *(p++);
    }
    *n = '\0';
    
    WHD_Util::cleanStr(val);
    if ((strlen(val) == 0) || !valSize) {
        return false;
    }
    
    return true;
}

bool DNOCR_Config::readNVPCheck (const char *name, const char *value)
{
    // returns error if file NVP does not match name:value
    // eg just to check version <Version:1.0> "Version", "1.0"
    
    char n[BUFFSIZE], v[BUFFSIZE];
    if (!parseUnarchiveLine (n, v, BUFFSIZE,  loadFile)){
        return false; // name:val corrupt
    }
    
    if ((strcmp(name, n ) != 0) || (strcmp(value, v) != 0)) {
        return false;
    }
    
    return true;
    
}


bool DNOCR_Config::readNVPString (const char *name, char *val, int valLength)
{
    // returns the value as a string
    char n[BUFFSIZE];
    if (!parseUnarchiveLine (n, val, valLength,  loadFile)){
        return false; // name:val corrupt
    }
    if (strcmp(name, n) != 0) {
        return false; // name mismatch
    }
    return true;
}

bool DNOCR_Config::readNVPInt (const char *name, int *val)
{
    char buff[BUFFSIZE];
    char *p = (char *)name;
    if (*p == '&') p++;  // sometimes we get "&intValue"
    
    if (!this->readNVPString(p, buff, BUFFSIZE)){
        return false;
    } else {
        WHD_Util::cleanNumStr(buff);
        if (strlen(buff ) == 0) return false;
        *val = atoi(buff);
        return true;
    }
}

bool DNOCR_Config::readNVPLong (const char *name, long *val)
{
    char buff[BUFFSIZE];
    char *p = (char *)name;
    if (*p == '&') p++;  // sometimes we get "&intValue"
    
    if (!this->readNVPString(p , buff, BUFFSIZE)){
        return false;
    } else {
        WHD_Util::cleanNumStr(buff);
        if (strlen(buff ) == 0) return false;
        *val = atol(buff);
        return true;
    }
}

bool DNOCR_Config::readNVPBool (const char *name, bool *val)
{
    char  buff[BUFFSIZE];
    char *p = (char *)name;
    if (*p == '&') p++;  // sometimes we get "&intValue"
    
    if (!this->readNVPString(p, buff, BUFFSIZE)){
        return false;
    } else {
        if (strlen(buff) == 0) return false;
        if (strlen(buff) < 4) return false;  // should be 'true' or 'false'
        *val = (strcmp(buff, "true") == 0);
        return true;
    }
}

bool DNOCR_Config::readNVPDateTime (const char * name,  datetimeInfo &dt)
{
    char buff [BUFFSIZE];
    if (!this->readNVPString(name, buff, BUFFSIZE)){
        return false;
    } else {
        if (strlen(buff) != 19) return false; // Must always be "yyyy-mm-dd-hh-mm-ss"
        sscanf (buff, "%d-%d-%d-%d-%d-%d", &dt.year, &dt.mon, &dt.day, &dt.hour, &dt.min, &dt.sec);
        return true;
    }
}


int DNOCR_Config::closeRecover()
{
    loadFile->close();
    return 1;
}