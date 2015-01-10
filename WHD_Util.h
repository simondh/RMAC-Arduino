//
//  WHD_Util.h
//  DNOCR_LIB
//
//  Created by Simon Hewitt on 07/01/2015.
//  Copyright (c) 2015 Simon Hewitt. All rights reserved.
//
//  Utility library , mostly string manupulation and other utilities not present in Arduino/LinkIt libraries.
//  Note the code is intended for the LinkitOne development board, a dialect of Arduino
//  Can be compiled in Xcode (and maybe other C++ IDEs) using the LFlash emulator library.
//  NOTE the STL is avaiulable for Arduino, may use this more in the future https://github.com/rpavlik/StandardCplusplus
//
//  Contact simon@whd-engineering.com
//

#ifndef __DNOCR_LIB__WHD_Util__
#define __DNOCR_LIB__WHD_Util__

// define XCODE to dev and test under XCODE, standard C++. Comment out for normal Arduino use
#define XCODE 1

class DNOCR_Config;


/* 
 ** Global constants
*/


// configuration files in Flash memory
#define CONFIGFILENAME "RMAC"
#define CONFIGFILEEXT ".DAT"
#define CONFIGBACKUPEXT ".BAK"
#define CONFIGDEFAULTEXT ".CFG"
#define LOG_WARN 'W'
#define LOG_INFO 'I'
#define LOG_ERROR 'E'


/*
 ** Global constants
 */
const int IDLength = 6;             // 5 chars plus \0
const int ShortNameLength = 9;      // 8 chars plus \0
const int LongNameLength = 33;      // 32 chars plus \0
const int MaxOnOffAlarms = 8;       // thats the number of digital channels built!
const int MaxAnalogueChannels = 3;  // ditto 3 analogue channels
const int MaxCertifiedPhones = 32;  // arbitrary limit
const int MaxPhoneLength = 16;      // ITU says 15 max, plus \0


/*
 ** WHD_Util - mostly staic utility routines
 */

class WHD_Util
{
public:
    static int safeCopy (char *dest, char *source, int l);
    // copies source to dest , l is length of dest.
    // Ensures no overflow and is null terminated, so at most l-1 characters copied
    
    static int cleanTelephone (char *tnum);
    // Cleans up telephone number in place to a string of contiguous digits, anything else discarded,
    // and ensures it startes with 0044 if no intl code specified
    // eg "07584 658227" -> "00447584658227", "(01792)-455806" -> 00441792455806
    // +34 123 4567 -> 00341234567, 0034 123 4567 -> 00341234567 etc
    // truncates to 15 chars if necessary
    
    static void cleanStr (char * s);
    // removes leading and trailing spaces, multiple spaces reduced to one, removes any nonprinting chars
    // MUST be null terminated
    
    static void cleanNumStr (char *s);
    // modifies str in place to a nice clean string, leading +- and . and digits only
    // not THAT clever 22.22.22 probably OK?
    
    static void writeLog (char logType, const char *s);   // /log/RMAC000001.LOG etc
    
    static void fatalError (const char *);  // if program really cannot progress - just sits there flashing LEDs!
    
    static int dayOfEpoch ();  // Returns number of days since 1 Jan 2015
    
};




#endif /* defined(__DNOCR_LIB__WHD_Util__) */
