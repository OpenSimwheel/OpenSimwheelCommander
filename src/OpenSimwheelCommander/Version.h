#ifndef VERSION_H
#define VERSION_H

#define VERSION_MAJOR 0
#define VERSION_MINOR 5
#define VERSION_BUILD 1
#define VERSION_QFE 0
//#define VERSION_BUILD_DATE "24/05/2005"
//#define VERSION_BUILD_TIME "08:37:55UTC"

#define _STR(x) #x
#define STR(x) _STR(x)
#define VERSION_NUMBER VERSION_MAJOR,VERSION_MINOR, VERSION_BUILD,VERSION_QFE
#define VERSION_STRING STR(VERSION_MAJOR) "." STR(VERSION_MINOR) "." STR(VERSION_BUILD) "." STR(VERSION_QFE)

#define VERSION_COMPANY "OpenSimwheel"
#define VERSION_COPYRIGHT "(c) OpenSimwheel 2014 &lt;bernhard.berger@gmail.com&gt;"
#define VERSION_BUILD_DATE_TIME VERSION_BUILD_DATE " - " VERSION_BUILD_TIME


#endif // VERSION_H
