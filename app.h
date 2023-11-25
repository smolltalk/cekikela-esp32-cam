#ifndef APP_H
#define APP_H

#include "cfgmgt.h"
#include "error.h"

#define APP_LOG "App"

#define uS_TO_S_FACTOR 1000000  /* Conversion factor for micro seconds to seconds */

void zzzzZZZZ();
statusCode runActions();
void setupAppConfig(appConfig_t * appConfig);

// TODO
// - No wifi param -> No NTP
// - Log
// - Write Readme.md
// - OTA
// - gainceiling, framesize, etc settings
// - use begin instead of initXYZ
// - simplify getting member address with offsetof
// - Rename file in C or use extern C { } as in sensor.h
// - SDConfig with section

#endif