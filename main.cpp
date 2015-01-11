//
//  main.cpp
//  DNOCR_LIB
//
//  Created by Simon Hewitt on 07/01/2015.
//  Copyright (c) 2015 Simon Hewitt. All rights reserved.
//

#include <iostream>
#include <string>
#include <fstream>
#include "WHD_Util.h"
#include "DNOCR_Config.h"
#include "DNOCR_Rmac.h"



// global data  - all the classes restored from config file.
Site siteData;
DigitalAlarm* digitalAlarms[MaxDigitalAlarms];
AuthorisedUsers *authUsers[MaxCertifiedPhones];


// Global const etc
#define DEBUG 1

/*
 **====================== functions used by main =================
 ** These functions are generally those that operate on soem or all global variables. 
 ** eg save & restore
 **/

void restoreAllFromConfig ()
{
    // Restores all classes.
    // and reload from data file
    int i;
    
    DNOCR_Config restoreObj;
    
    
    restoreObj.startRestoreConfig(); // if there are no config files, this never returns!
    // restire the main Site object first
    siteData.unarchive(&restoreObj);
    siteData.debugON = DEBUG;
    WHD_Util::writeLog(LOG_INFO, "Single SITE object restored");

    
    // then all digitalAlarm objects (the no-volt switch, digital pins)
    for (i = 0; i < MaxDigitalAlarms; i++)
    {
        digitalAlarms[i] = new DigitalAlarm(i+1);
        digitalAlarms[i]->unarchive(&restoreObj, i+1);
    }
    WHD_Util::writeLog(LOG_INFO, "All Digital Alarms restored");

    
    // All authorised users next
    for (i = 0; i < MaxCertifiedPhones; i++)
    {
        authUsers[i] = new AuthorisedUsers(i+1);
        authUsers[i]->unarchive(&restoreObj, i+1);
    }
    WHD_Util::writeLog(LOG_INFO, "All Authorised users restored");

    
} // restoreAllFromConfig

void saveAllToConfig ()
{
    // SAVES all classes to a new config file.
    int i;
    DNOCR_Config saveObj;

    WHD_Util::writeLog(LOG_INFO, "Saving ALL classes to RMAC.DAT");

    // Save main site object first
    saveObj.startSave();
    siteData.archive(&saveObj);
    
    // All digital alarms next
    for (i = 0; i < MaxDigitalAlarms; i++)
    {
        digitalAlarms[i]->archive (&saveObj);
    }
    
    // All authorised users next
    for (i = 0; i < MaxCertifiedPhones; i++)
    {
        authUsers[i]->archive (&saveObj);
    }
    // Finally close the file
    saveObj.closeSave();
    WHD_Util::writeLog(LOG_INFO, "All classes saved");

} // saveAllToConfig ()


/*
 **====================== main =================
 **/

int main(int argc, const char * argv[]) {
    

    //int i;
    //char buff [BUFSIZ];
    
    WHD_Util::writeLog(LOG_INFO, "RMAC starting up");


    // FIRST get data from teh config files.
    // IF there are no config files, it never returns and flashes LEDs forever
    WHD_Util::writeLog(LOG_INFO, "Loading all classes from config file RMAC.DAT");

    restoreAllFromConfig();
    
    WHD_Util::writeLog(LOG_INFO, "restoration complete, startup routines starting");


 
    
    saveAllToConfig();
    
    return 0;
}
