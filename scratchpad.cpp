//
//  scratchpad.cpp
//  DNOCR_LIB
//
//  Created by Simon Hewitt on 11/01/2015.
//  Copyright (c) 2015 Simon Hewitt. All rights reserved.
//
// THIS IS JUST A SCRATCH PAD FOR CODE I DONT WANT TO THROW AWAY YET

#include "scratchpad.h"


void alarmDummyText (DigitalAlarm *alm)
{
    // debug, add some text to init the config file
    int a = alm->alarmNumber;
    
    sprintf (alm->alarmID, "A%03d", alm->alarmNumber);
    sprintf (alm->alarmName, "Alarm %03d", a);
    alm->digitalPin = 11 - a;
    alm->alarmOnOpen = (a%2);
    alm->silence = (a%4);
    alm->retransmitMinutes = a * 15;
    alm->alarmed = false;
    alm->globalAlarmCount = a * 99;
}

//    while (more) {
//
//        std::cout << "Enter a string to be cleaned or exit\n" << std::endl;
//        std::getline (std::cin, b2);
//        c = b2.c_str();
//        strcpy(buf, c);
//        std::cout << buf << std::endl;
//
//        if (strcmp("exit", buf) == 0) exit(0);
//        WHD_Util::cleanNumStr(buf);
//        std::cout << buf << std::endl;
//
//    }

//    char buf[257];
//    std::string b2;
//    int more = 1;
//    const char * c ;


//    siteData.setSiteID((char*)("00001"));
//    siteData.setSiteShortName ((char*)("SW-East"));
//    siteData.setSiteLongName((char*)("Swansea east"));
//    siteData.setLowSecurityPIN((char*)("11111"));
//    siteData.setHighSecurityPIN((char*)("99999"));


// temp debug - build template auth users
for (i = 0; i < MaxCertifiedPhones; i++)
{
    authUsers[i] = new AuthorisedUsers(i+1);
    sprintf  (authUsers[i]->userName, "User-%03d", i+1);
    sprintf  (authUsers[i]->phone, "%03d%02d%05d", i+1, i+4, i+9);
    switch (i%5) {
        case 0:  authUsers[i]->securityLevel = sec_None; break;
        case 1:  authUsers[i]->securityLevel = sec_RO; break;
        case 2:  authUsers[i]->securityLevel = sec_Low; break;
        case 3:  authUsers[i]->securityLevel = sec_High; break;
        case 4:  authUsers[i]->securityLevel = sec_Admin; break;
            
        default:
            break;
    }
}




//        int err;
//
//        err = myQ->enQueue((char *)""); say(myQ);
//        err = myQ->enQueue((char *)"abcd");say(myQ);
//        err = myQ->enQueue((char *)" 11111 ");say(myQ);
//        err = myQ->enQueue((char *)" 11111 S??  ");say(myQ);
//        err = myQ->enQueue((char *)"11111 S?? 1  ");say(myQ);
//    err = myQ->enQueue((char *)"  11111   S?? 1 2 ");say(myQ);
//    err = myQ->enQueue((char *)"  11111   S?? 1 1234567890123456789012345678901234567890xx ");say(myQ);
//    err = myQ->enQueue((char *)"  11111   S?? 1 2 3");say(myQ);
//    err = myQ->enQueue((char *)"  11111   S?? 1 2 3 4 5 6 7 8 9");say(myQ);
//    err = myQ->enQueue((char *)"  11111   S?? 1 2 3 4 5 6 7 8 9 10");say(myQ);
//    err = myQ->enQueue((char *)"  11111   S?? 1 2 3 4 5 6 7 8 9 10 11");say(myQ);
//    err = myQ->enQueue((char *)"  11111   S?? 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20");say(myQ);
//    err = myQ->enQueue((char *)"  11111   S?? 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21");say(myQ);
//    err = myQ->enQueue((char *)"  11111   S?? 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22");say(myQ);





/**
 **
 ** Setter and Getter methods used soley to ensure no buffer overflow, and restrict numeric ranges in some cases
 **
 **/

//const char * Site::getSiteID() const
//{
//    return (siteID);
//}
//
//
//int Site::setSiteID(const char *name)
//{
//    // fussy, but make sure a) Name cannot overflow, and b) is null terminated
//    return (WHD_Util::safeCopy(siteID, (char *)name, sizeof(siteID)));
//}
//
//const char * Site::getSiteShortName() const
//{
//    return(siteShortName);
//}
//
//int Site::setSiteShortName(const char* name)
//{
//    return (WHD_Util::safeCopy(siteShortName, (char *)name, sizeof(siteShortName)));
//}
//
//const char * Site::getSiteLongName() const
//{
//    return(siteLongName);
//}
//
//int Site::setSiteLongName(const char* name)
//{
//    return (WHD_Util::safeCopy(siteLongName, (char *)name, sizeof(siteLongName)));
//}
//
//const char * Site::getLowSecurityPIN() const
//{
//    return(lowSecurityPIN);
//}
//
//int Site::setLowSecurityPIN(const char* name)
//{
//    return (WHD_Util::safeCopy(lowSecurityPIN, (char *)name, sizeof(lowSecurityPIN)));
//}
//
//const char * Site::getHighSecurityPIN() const
//{
//    return(highSecurityPIN);
//}
//
//int Site::setHighSecurityPIN(const char* name)
//{
//    return (WHD_Util::safeCopy(highSecurityPIN, (char *)name, sizeof(highSecurityPIN)));
//}

