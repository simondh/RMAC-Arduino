//
//  DNOCR_Rmac.cpp
//  DNOCR_LIB
//
//  Created by Simon Hewitt on 07/01/2015.
//  Copyright (c) 2015 Simon Hewitt. All rights reserved.
//
// All main Configuration classes:
// Site
// DigitalAlarms
// AuthorisedUsers
//
//
// With archive and unarchive methiods for them alloca(


#include <cstring>
#include <ctype.h>
#include <ctime>
#include "DNOCR_Config.h"
#include "DNOCR_Rmac.h"
#include "WHD_Util.h"

// Edit these version numbers very time an attribute is added to a class.
// NEVER remove, resequence or alter existing attributes
// or the config files will crash horribly
#define SITE_VERSION "1.0"      
#define DIGITAL_ALARM_VERSION "1.0"      
#define AUTH_USERS_VERSION "1.0"



// nasty nasty macros as I am tired of typing
// RNC3 for the ReadNVP{type} methods that take 3 params, ("attribute-name", attribute, sizeof(attribute)
// RNC2 where sizef not needed, like ReadNVPBool
// RNC3Q where 2 quoted strings needed, simple ReadNVPCheck (string, string)
#define PACK(str) #str
#define QUOTE(str) PACK(str)

#define RNC3(TYPE, ID) if (!restoreObj->readNVP##TYPE (QUOTE(ID), ID, sizeof(ID))) localFatalError ("DigitalAlarm", QUOTE(ID));
#define RNC2(TYPE, ID) if (!restoreObj->readNVP##TYPE (QUOTE(ID), ID)) localFatalError ("DigitalAlarm", QUOTE(ID));
#define RNC3Q(TYPE, C, ID) if (!restoreObj->readNVP##TYPE (QUOTE(C), QUOTE(ID))) localFatalError ("DigitalAlarm", QUOTE(ID));


/* A note on the F() macro - this tells Arduino (and LinkIt I hope) to keep a character string in Flash until needed.
  so does not use up RAM */


/*
 **------------------------------------------------------------------------------------
 ** class Site - describes the overall site.
 ** setters defined just to ensure no buffer overflow (as Arduino does not support String!)
 **/

Site::Site()
{
    // Constructor. All it does (now) is put timestamp in the object
    
    LDateTimeClass::getTime(&bootupTime);
}

void Site::archive (DNOCR_Config *saveObj)
{
    // saveObj class has a pointer to the file, and methods to write the data
    // This method writes the class data out in attribute_name::data pairs, one per line
    // Note the name and attribute name do not HAVE to be the same but life is much easier if they are!
    // MAKE SURE the unarchive processes the SAME attributes in the SAME order
    
    saveObj->writeNVP("Class", "Site");
    saveObj->writeNVP("Version", SITE_VERSION);
    saveObj->writeNVP("siteID", siteID);
    saveObj->writeNVP("siteShortName", siteShortName );
    saveObj->writeNVP("siteLongName", siteLongName );
    saveObj->writeNVP("lowSecurityPIN", lowSecurityPIN );
    saveObj->writeNVP("highSecurityPIN",  highSecurityPIN);
    saveObj->writeNVP("debugON",  debugON);
    //saveObj->writeNVP("x", x );
    saveObj->writeNVP("logFileNumber", (long)logFileNumber);
    saveObj->writeSaveLine("ENDClass:Site");
    saveObj->writeSaveLine("//------");
    saveObj->writeSaveLine("");

    
}

void Site::unarchive (DNOCR_Config *restoreObj)
{
    // The unarchive file is open, and the next line should be Class:Site,
    // followed by the values for eachg attribute EXACTLY as for Archive (above)
    // and sadly Arduino does not do try/throw/catch, so its a big ugly
    
    if (!restoreObj->readNVPCheck("Class", "Site"))
        WHD_Util::fatalError (F("Site unarchive first line NOT Class"));
    if (!restoreObj->readNVPCheck("Version", SITE_VERSION))
        WHD_Util::fatalError(F("Read Config file fail, bad Site Version"));
    if (!restoreObj->readNVPString("siteID", siteID, sizeof(siteID)))
        WHD_Util::fatalError(F("Read Config file fail, bad Site ID"));
    if (!restoreObj->readNVPString("siteShortName", siteShortName , sizeof(siteShortName)))
        WHD_Util::fatalError(F("Read Config file fail, bad Site Short Name"));
    if (!restoreObj->readNVPString("siteLongName", siteLongName, sizeof(siteLongName)))
        WHD_Util::fatalError(F("Read Config file fail, bad Site Long Name"));
    if (!restoreObj->readNVPString("lowSecurityPIN", lowSecurityPIN, sizeof(lowSecurityPIN)))
        WHD_Util::fatalError(F("Read Config file fail, bad Low Security Pin"));
    if (!restoreObj->readNVPString("highSecurityPIN",  highSecurityPIN, sizeof(highSecurityPIN)))
        WHD_Util::fatalError(F("Read Config file fail, bad High Security Pin"));
    if (!restoreObj->readNVPBool ("debugON",  &debugON))
        WHD_Util::fatalError(F("Read Config file fail, bad Debug flag"));
    if (!restoreObj->readNVPInt ("logFileNumber", &logFileNumber))
        WHD_Util::fatalError(F("Read Config file fail, bad Log File Number"));
    if (!restoreObj->readNVPCheck("ENDClass", "Site"))
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


/*
 **------------------------------------------------------------------------------------
 ** DigitalAlarm
 **/

void zeroDT (datetimeInfo *dt)
{
    dt->year = 0;
    dt->mon = 0;
    dt->day = 0;
    dt->hour = 0;
    dt->min = 0;
    dt->sec = 0;
}


DigitalAlarm::DigitalAlarm(int alarmNum)
{
    // Each object must be numbered on create
    
    alarmNumber = alarmNum;
    alarmed = false;
    alarmRaisedTime = *new datetimeInfo();
    alarmClearedTime = *new datetimeInfo();
    //zeroDT (&alarmRaisedTime);
    //zeroDT (&alarmClearedTime);
}

void DigitalAlarm::archive (DNOCR_Config *saveObj)
{
    // saveObj class has a pointer to the file, and methods to write the data
    // This method writes the class data out in attribute_name::data pairs, one per line
    // Note the name and attribute name do not HAVE to be the same but life is much easier if they are!
    // MAKE SURE the unarchive processes the SAME attributes in the SAME order
    
    if (this == NULL) WHD_Util::fatalError("DigitalAlarm::archive THIS is null");

    saveObj->writeNVP("Class", "DigitalAlarm");
    saveObj->writeNVP("Version", DIGITAL_ALARM_VERSION);
    
    saveObj->writeNVP("alarmNumber", (long)alarmNumber);
    saveObj->writeNVP("alarmID", alarmID);
    saveObj->writeNVP("alarmName", alarmName);
    saveObj->writeNVP("digitalPin", (long)digitalPin);
    saveObj->writeNVP("alarmOnOpen", alarmOnOpen);
    saveObj->writeNVP("silence", silence);
    saveObj->writeNVP("retransmitMinutes", (long)retransmitMinutes);
    saveObj->writeNVP("alarmed", alarmed);
    saveObj->writeNVP("alarmRaisedTime", alarmRaisedTime);
    saveObj->writeNVP("alarmClearedTime", alarmClearedTime);
    saveObj->writeNVP("globalAlarmCount", globalAlarmCount);
    saveObj->writeSaveLine("ENDClass:DigitalAlarm");
    saveObj->writeSaveLine("//------");
    saveObj->writeSaveLine("");
  
    //    saveObj->writeNVP("xx", xx);  // template

}



void localFatalError (const char *className, const char *id)
{
    char buff[BUFSIZ], *p;
    p = (char *)id;
    if (id[0] == '&') p++;  // some attributes are &attribute, ignore this
    sprintf (buff, "Class: %s unarchive error on attribute: %s", className, p);
    WHD_Util::fatalError(buff);
}

void DigitalAlarm::unarchive (DNOCR_Config *restoreObj, const int restoreAlarmNumber)
{
    // The unarchive file is open, and the next line should be Class::DigitalAlarm,
    // followed by the values for eachg attribute EXACTLY as for Archive (above)
    // and sadly Arduino does not do try/throw/catch, so its a big ugly
    // Note there are many alarms -
    // we are expilicity restoring number restoreAlarmNumber, fatal if not (1-based)
    
    char buff[BUFSIZ];
    if (this == NULL) WHD_Util::fatalError("DigitalAlarm::unarchive THIS is null");

    RNC3Q(Check, Class, DigitalAlarm);
    if (!restoreObj->readNVPCheck("Version", DIGITAL_ALARM_VERSION))
        localFatalError("Version", DIGITAL_ALARM_VERSION);
    //RNC3Q(Check, Version, DIGITAL_ALARM_VERSION);
    RNC2(Int, &alarmNumber);
    if (alarmNumber != restoreAlarmNumber) {
        sprintf (buff, "unarchive DigitalAlarm expected %d got %d", restoreAlarmNumber, alarmNumber);
        WHD_Util::fatalError(buff);
    }
    RNC3(String, alarmID);
    RNC3(String, alarmName);
    RNC2(Int, &digitalPin);
    RNC2(Bool, &alarmOnOpen);
    RNC2(Bool, &silence);
    RNC2(Int , &retransmitMinutes);
    RNC2(Bool, &alarmed);
    RNC2(DateTime , alarmRaisedTime);
    RNC2(DateTime , alarmClearedTime);
    RNC2(Long  , &globalAlarmCount);
    RNC3Q(Check, ENDClass, DigitalAlarm);
}



/*
 **------------------------------------------------------------------------------------
 ** AuthorisedUsers
 **/

AuthorisedUsers::AuthorisedUsers (const int unum)
{
    if ((unum < 1) || (unum > MaxCertifiedPhones))
        WHD_Util::fatalError("AuthorisedUser number out of range 1..32");
    
    userNumber = unum;
}



void AuthorisedUsers::archive (DNOCR_Config *saveObj)
{
    // saveObj class has a pointer to the file, and methods to write the data
    // This method writes the class data out in attribute_name::data pairs, one per line
    // Note the name and attribute name do not HAVE to be the same but life is much easier if they are!
    // MAKE SURE the unarchive processes the SAME attributes in the SAME order
    
    if (this == NULL) WHD_Util::fatalError("Authorised_Users::archive THIS is null");

    saveObj->writeNVP("Class", "AuthorisedUsers");
    saveObj->writeNVP("Version", AUTH_USERS_VERSION);
    
    saveObj->writeNVP("userNumber", (long)userNumber);
    saveObj->writeNVP("userName", userName);
    saveObj->writeNVP("phone", phone);
    saveObj->writeNVP("securityLevel", (long)securityLevel);
    saveObj->writeSaveLine("ENDClass:AuthorisedUsers");

    saveObj->writeSaveLine("//------");
    saveObj->writeSaveLine("");
}

void AuthorisedUsers::unarchive (DNOCR_Config *restoreObj, const int uNum)
{
    // The unarchive file is open, and the next line should be Class::DigitalAlarm,
    // followed by the values for eachg attribute EXACTLY as for Archive (above)
    // and sadly Arduino does not do try/throw/catch, so its a big ugly
    // Note there are many alarms -
    // we are expilicity restoring number restoreAlarmNumber, fatal if not (1-based)
    
    char buff[BUFSIZ];
    int i;
    
    if (this == NULL) WHD_Util::fatalError("Authorised_Users::unarchive THIS is null");
    
    RNC3Q(Check, Class, AuthorisedUsers);
    if (!restoreObj->readNVPCheck("Version", AUTH_USERS_VERSION))
        localFatalError("Version", AUTH_USERS_VERSION);
    RNC2(Int, &userNumber);
    if (uNum != userNumber) {
        sprintf (buff, "unarchive Authorised User expected %d got %d", uNum, userNumber);
        WHD_Util::fatalError(buff);
    }
    RNC3(String, userName);
    RNC3(String, phone);
    if (!restoreObj->readNVPInt ("securityLevel", &i))
        WHD_Util::fatalError(F("Read Config file fail, bad securityLevel Number"), i );
    securityLevel = (SecurityLevels)i;

    RNC3Q(Check, ENDClass, AuthorisedUsers);
}




/*
 **------------------------------------------------------------------------------------
 ** ??
 **/

