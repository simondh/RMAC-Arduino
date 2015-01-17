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
#include "LFlash.h"

// Edit these version numbers very time an attribute is added to a class.
// NEVER remove, resequence or alter existing attributes
// or the config files will crash horribly
#define SITE_VERSION "1.0"
#define DIGITAL_ALARM_VERSION "1.0"
#define AUTH_USERS_VERSION "1.0"



// nasty nasty macros as I am tired of typing
// shorten the repetitive pattern in restore methods:
// if (!restoreObj->readNVPInt ("alarmNum", alarmNum))
//      WHD_Util::fatalError ("Restoring class_type error parameter:, "alarmNum");
//
// TODO: Here fix fatalError text
// note NO quotes in macro parameters, screws it up a treat.
// RNC3 for the ReadNVP{type} methods that take 3 params, ("attribute-name", attribute, sizeof(attribute)
//      (only 2 params supplied, the Type and attribute
//      eg RNC3 (char, paramName) ->
//      ->readNVPchar ("paramName", paramName, sizeof(paramName)
// RNC2 where sizef not needed, like ReadNVPBool
// RNC3Q where 2 quoted strings needed, simple ReadNVPCheck (string, string)
//  the double-macro parsing puts quotes around the macro value.

#define PACK(str) #str
#define QUOTE(str) PACK(str)

#define RNC3(TYPE, ID) if (!restoreObj->readNVP##TYPE (QUOTE(ID), ID, sizeof(ID))) WHD_Util::fatalError ("DigitalAlarm", QUOTE(ID));
#define RNC2(TYPE, ID) if (!restoreObj->readNVP##TYPE (QUOTE(ID), ID)) WHD_Util::fatalError ("DigitalAlarm", QUOTE(ID));
#define RNC3Q(TYPE, C, ID) if (!restoreObj->readNVP##TYPE (QUOTE(C), QUOTE(ID))) WHD_Util::fatalError ("DigitalAlarm", QUOTE(ID));


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
    
    configDirty = false;
    
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
    
    configDirty = false;
}

bool Site::pinOK (char *pin, SecurityLevels securityLevel)
{
    switch (securityLevel) {
        case sec_Low:
            return strncmp(pin, lowSecurityPIN, sizeof(lowSecurityPIN)-1);
            break;
            
        case sec_High:
            return strncmp(pin, highSecurityPIN, sizeof(highSecurityPIN)-1);
            break;
            
        default:
            return false;
            break;
    }
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
}

bool DigitalAlarm::isActive()
{
    return active;
}

void DigitalAlarm::sendDigitalAlarm(const char *typeOfAlarm)
{
    // does nothing yet!!
    lastTransmitted.setArdTimeNow();

}

void DigitalAlarm::setupAlarm()
{
    pinMode(digitalPin, INPUT);
    alarmed = false;
    lastTransmitted.setArdTimeZero();
    alarmRaisedTime.setArdTimeZero();
    alarmClearedTime.setArdTimeZero();
    sessionAlarmCount = 0;
    bounceCount = 0;
    lastChecked = millis();
}

void DigitalAlarm::checkAlarm (Site *site)
{
    if (!active) return;  // should not happen anyway...
    if (WHD_Util::millisSince(lastChecked) < checkInterval) return;
    // only check at set intervals.
    
    lastChecked = millis();
    
    int state = digitalRead (digitalPin); // TODO: actually more complicated, depends on the pin?
    
    if ((alarmOnOpen && (state == 0)) || (!alarmOnOpen && (state != 0)))
    {
        // YES there is an alarm state.
        if (bounceCount > 0) {
            bounceCount--;
            return; // BUT ignore it until it has been present for bounceCount intervals
        }
        
        if (alarmed) {
            // alarm has already been raised - resignal it?
            if (!silence && ((lastTransmitted.secondsDifference()) > retransmitMinutes )){
                // resend the alarm
                sendDigitalAlarm("Repeat");
            }  // else ignore, alarm has been sent and nothing to do
        } else {
            // NEW alarm!!
            if (!silence){
                sendDigitalAlarm("New");
                WHD_Util::writeLog(LOG_ALARM, "Alarm RAISED ", alarmID);
            } else {
                WHD_Util::writeLog(LOG_ALARM, "Alarm RAISED but Silenced", alarmID);
                alarmRaisedTime.setArdTimeZero(); // as not transmitted!
            }
            alarmed = true;
            alarmRaisedTime.setArdTimeNow();
            sessionAlarmCount++;
            globalAlarmCount++;
        }
        
    } else {
        // NOT alarmed, OK - but - do we need to reset an active alarm?
        if (alarmed) {
            // clear the alarm
            if (!silence){
                sendDigitalAlarm("reset");
                WHD_Util::writeLog(LOG_ALARM, "Alarm cleared", alarmID);
            } else {
                WHD_Util::writeLog(LOG_ALARM, "Alarm Cleared but Silenced", alarmID);
            }
            alarmed = false;
            alarmClearedTime.setArdTimeNow();
            bounceCount = deBounce;
        }
    }
    
    
    
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
    saveObj->writeNVP("active", active);
    saveObj->writeNVP("digitalPin", (long)digitalPin);
    saveObj->writeNVP("alarmOnOpen", alarmOnOpen);
    saveObj->writeNVP("silence", silence);
    saveObj->writeNVP("retransmitMinutes", (long)retransmitMinutes);
    saveObj->writeNVP("alarmed", alarmed);
    saveObj->writeNVP("alarmRaisedTime", alarmRaisedTime);
    saveObj->writeNVP("alarmClearedTime", alarmClearedTime);
    saveObj->writeNVP("globalAlarmCount", globalAlarmCount);
    saveObj->writeNVP("deBounce", (long)deBounce);
    saveObj->writeNVP("checkInterval", (long)checkInterval);
    
    saveObj->writeSaveLine("ENDClass:DigitalAlarm");
    saveObj->writeSaveLine("//------");
    saveObj->writeSaveLine("");
    
    //    saveObj->writeNVP("xx", xx);  // template
    
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
        WHD_Util::fatalError("Version", DIGITAL_ALARM_VERSION);
    
    RNC2(Int, &alarmNumber);
    if (alarmNumber != restoreAlarmNumber) {
        sprintf (buff, "unarchive DigitalAlarm expected %d got %d", restoreAlarmNumber, alarmNumber);
        WHD_Util::fatalError(buff);
    }
    RNC3(String, alarmID);
    RNC3(String, alarmName);
    RNC2(Bool, &active);
    RNC2(Int, &digitalPin);
    RNC2(Bool, &alarmOnOpen);
    RNC2(Bool, &silence);
    RNC2(Int , &retransmitMinutes);
    RNC2(Bool, &alarmed);
    RNC2(DateTime , alarmRaisedTime);
    RNC2(DateTime , alarmClearedTime);
    RNC2(Long  , &globalAlarmCount);
    RNC2(Int  , &deBounce);
    RNC2(Int  , &checkInterval);
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
    saveObj->writeNVP("active", active);
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
        WHD_Util::fatalError("Version", AUTH_USERS_VERSION);
    RNC2(Int, &userNumber);
    if (uNum != userNumber) {
        sprintf (buff, "unarchive Authorised User expected %d got %d", uNum, userNumber);
        WHD_Util::fatalError(buff);
    }
    RNC2(Bool , &active);
    RNC3(String, userName);
    RNC3(String, phone);
    if (!restoreObj->readNVPInt ("securityLevel", &i))
        WHD_Util::fatalError(F("Read Config file fail, bad securityLevel Number"), i );
    securityLevel = (Site::SecurityLevels)i;
    
    RNC3Q(Check, ENDClass, AuthorisedUsers);
}




/*
 **------------------------------------------------------------------------------------
 ** ??
 **/

