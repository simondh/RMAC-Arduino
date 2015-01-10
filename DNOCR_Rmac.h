//
//  DNOCR_Rmac.h
//  DNOCR_LIB
//
//  Created by Simon Hewitt on 07/01/2015.
//  Copyright (c) 2015 Simon Hewitt. All rights reserved.
//
//  This is the DNOC Remote Monitoring main library.
//  It contains classes that define the main entities such as unit, alarm, securityEntities
//  and general utilities
//  Note the code is intended for the LinkitOne development board, a dialect of Arduino
//  and can only be compiled on an Arduino IDE with the Linkit One extension.
//
//  Contact simon@whd-engineering.com
//

#ifndef __DNOCR_LIB__DNOCR_Rmac__
#define __DNOCR_LIB__DNOCR_Rmac__

// define XCODE to dev and test under XCODE, standard C++. Comment out for normal Arduino use
#define XCODE 1

class DNOCR_Config;





/*
 ** class Site
 ** There should be only one, as it defines the entire site 
 ** (but see long StackOverflow discussion on singletons , decided not to use singleton, just don't define it twice!)
 ** Contains all site wide data, and methods to serrialize /userialize to/from Flash file.
*/

class Site
{
public:
    // Methods
    const char * getSiteID() const;
    int setSiteID(const char*);
    const char * getSiteShortName()const ;
    int setSiteShortName(const char*);
    const char * getSiteLongName()const ;
    int setSiteLongName(const char*);
    const char * getLowSecurityPIN()const ;
    int setLowSecurityPIN(const char*) ;
    const char * getHighSecurityPIN()const ;
    int setHighSecurityPIN(const char*);
    void archive (DNOCR_Config *saveFile);
    void unarchive (DNOCR_Config *restoreFile);

    
    // Public attributes
    int configDirty = 0;        // set to 1 on any config change, reset to 0 when saved to config file
    bool debugON = true;        // set to false for no debug
    int  logFileNumber = 0;     // Simple incrementing number to suffix log files
    static int logFileDay;      // new log is created on startup and on day change. This is day of year used for that

private:
    char siteID[IDLength];              // any string to ID the site
    char siteShortName[ShortNameLength];// short names for SMS use
    char siteLongName[LongNameLength];  // long name for later Web use
    char lowSecurityPIN[IDLength];      // low security PIN. Not to be confused with a phyiscal electrical PIN
    char highSecurityPIN[IDLength];     // high security PIN
    
};




#endif /* defined(__DNOCR_LIB__DNOCR_Rmac__) */
