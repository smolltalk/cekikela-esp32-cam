#ifndef CONFIG_H
#define CONFIG_H

#include "cfgmgt.h"

RTC_DATA_ATTR appConfig_t appConfig = {
  .readConfigOnSdCard = true,
  .configOnSdCardRead = false,
  .cameraSetting = {
    .sensorSettingCount = 0
  }
};

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