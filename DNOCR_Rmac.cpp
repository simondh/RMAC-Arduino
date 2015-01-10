//
//  DNOCR_Rmac.cpp
//  DNOCR_LIB
//
//  Created by Simon Hewitt on 07/01/2015.
//  Copyright (c) 2015 Simon Hewitt. All rights reserved.
//

#include <cstring>
#include <ctype.h>
#include <ctime>
#include "DNOCR_Config.h"
#include "DNOCR_Rmac.h"
#include "WHD_Util.h"

#define SITE_VERSION "1.0"      // Edit this EVERY time the class attributes are altered
#define CRLF "\r\n"

/*
 ** class Site - describes the overall site.
 ** setters defined just to ensure no buffer overflow (as Arduino does not support String!)
*/

/* A note on the F() macro - this tells Arduino (and LinkIt I hope) to keep a character string in Flash until needed.
  so does not use up RAM */


void Site::archive (DNOCR_Config *saveFile)
{
    // saveFile class has a pointer to the file, and methods to write the data
    // This method writes the class data out in attribute_name::data pairs, one per line
    // Note the name and attribute name do not HAVE to be the same but life is much easier if they are!
    // MAKE SURE the unarchive processes the SAME attributes in the SAME order
    
    saveFile->writeSaveLine("Class:Site");
    saveFile->writeNVP("Version", SITE_VERSION);
    saveFile->writeNVP("siteID", siteID);
    saveFile->writeNVP("siteShortName", siteShortName );
    saveFile->writeNVP("siteLongName", siteLongName );
    saveFile->writeNVP("lowSecurityPIN", lowSecurityPIN );
    saveFile->writeNVP("highSecurityPIN",  highSecurityPIN);
    saveFile->writeNVP("debugON",  debugON);
    //saveFile->writeNVP("x", x );
    saveFile->writeNVP("logFileNumber", logFileNumber);
    saveFile->writeSaveLine("ENDClass:Site");

    
}

void Site::unarchive (DNOCR_Config *restoreFile)
{
    // The unarchive file is open, and the next line should be Class:Site,
    // followed by the values for eachg attribute EXACTLY as for Archive (above)
    // and sadly Arduino does not do try/throw/catch, so its a big ugly
    
    if (!restoreFile->readNVPCheck("Class", "Site"))
        WHD_Util::fatalError (F("Site unarchive first line NOT Class"));
    if (!restoreFile->readNVPCheck("Version", SITE_VERSION))
        WHD_Util::fatalError(F("Read Config file fail, bad Site Version"));
    if (!restoreFile->readNVPString("siteID", siteID, sizeof(siteID)))
        WHD_Util::fatalError(F("Read Config file fail, bad Site ID"));
    if (!restoreFile->readNVPString("siteShortName", siteShortName , sizeof(siteShortName)))
        WHD_Util::fatalError(F("Read Config file fail, bad Site Short Name"));
    if (!restoreFile->readNVPString("siteLongName", siteLongName, sizeof(siteLongName)))
        WHD_Util::fatalError(F("Read Config file fail, bad Site Long Name"));
    if (!restoreFile->readNVPString("lowSecurityPIN", lowSecurityPIN, sizeof(lowSecurityPIN)))
        WHD_Util::fatalError(F("Read Config file fail, bad Low Security Pin"));
    if (!restoreFile->readNVPString("highSecurityPIN",  highSecurityPIN, sizeof(highSecurityPIN)))
        WHD_Util::fatalError(F("Read Config file fail, bad High Security Pin"));
    if (!restoreFile->readNVPBool ("debugON",  &debugON))
        WHD_Util::fatalError(F("Read Config file fail, bad Debug flag"));
    if (!restoreFile->readNVPInt ("logFileNumber", &logFileNumber))
        WHD_Util::fatalError(F("Read Config file fail, bad Log File Number"));
    if (!restoreFile->readNVPCheck("ENDClass", "Site"))
        WHD_Util::fatalError(F("Read Config file fail, missing Site EndClass"));

}

const char * Site::getSiteID() const
{
    return (siteID);
}


int Site::setSiteID(const char *name)
{
    // fussy, but make sure a) Name cannot overflow, and b) is null terminated
    return (WHD_Util::safeCopy(siteID, (char *)name, sizeof(siteID)));
}

const char * Site::getSiteShortName() const
{
    return(siteShortName);
}

int Site::setSiteShortName(const char* name)
{
    return (WHD_Util::safeCopy(siteShortName, (char *)name, sizeof(siteShortName)));
}

const char * Site::getSiteLongName() const
{
    return(siteLongName);
}

int Site::setSiteLongName(const char* name)
{
    return (WHD_Util::safeCopy(siteLongName, (char *)name, sizeof(siteLongName)));
}

const char * Site::getLowSecurityPIN() const
{
    return(lowSecurityPIN);
}

int Site::setLowSecurityPIN(const char* name)
{
    return (WHD_Util::safeCopy(lowSecurityPIN, (char *)name, sizeof(lowSecurityPIN)));
}

const char * Site::getHighSecurityPIN() const
{
    return(highSecurityPIN);
}

int Site::setHighSecurityPIN(const char* name)
{
    return (WHD_Util::safeCopy(highSecurityPIN, (char *)name, sizeof(highSecurityPIN)));
}

