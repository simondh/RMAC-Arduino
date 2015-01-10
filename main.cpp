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

// global SITE class
Site site;

int main(int argc, const char * argv[]) {
    
//    char buf[257];
//    std::string b2;
//    int more = 1;
//    const char * c ;
    site.debugON = 1;
    
    std::cout << "DNOCR Library test skeleton\n" << std::endl;
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
    
    std::cout << "Testing class archive\n" << std::endl;
    
    // first create empty Site record
    Site siteData;
    // and reload from data file
    DNOCR_Config restoreFile;
    restoreFile.startRestoreConfig(); // if there are no config files, this never returns!
    siteData.unarchive(&restoreFile);
    
    
    
//    siteData.setSiteID((char*)("00001"));
//    siteData.setSiteShortName ((char*)("SW-East"));
//    siteData.setSiteLongName((char*)("Swansea east"));
//    siteData.setLowSecurityPIN((char*)("11111"));
//    siteData.setHighSecurityPIN((char*)("99999"));
    
    
    DNOCR_Config saveFile;
    
    WHD_Util::writeLog(LOG_INFO, "RMAC starting up");
    // Now save it
    saveFile.startSave();
    siteData.archive(&saveFile);
    saveFile.closeSave();
    
    
    
    return 0;
}
