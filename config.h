#ifndef CONFIG_H
#define CONFIG_H

#include "cfgmgt.h"

RTC_DATA_ATTR appConfig_t appConfig = {
  .readConfigOnSdCard = true,
  .configOnSdCardRead = false,
  .savePictureOnSdCard = true,
  .uploadPicture = true,
  .cameraGetReadyDurationMs = 1500,
  .awakeDurationMs = 5000,
  .wifiInfo = {
    {.ssid = "Freebox-778319"},
    {.password = "xxt6wq59kfqk63hdqxxzkm"},
    .connectAttemptMax = 30
  },
  .uploadInfo = {
    {.serverName = "37.27.8.236"},
    .serverPort = 80,
    {.uploadPath = "/upload.php"},
    {.auth = "bigblackbird"},
    .bunchSize = 2,
    .fileNameRandSize = 5
  }
};

// TODO
// - Write Readme.md
// - Make possible to take pictures periodically
// - OTA
// - DURATION_WAKEUP ? ON ISR ?
// - Add camera parameters to config
// - Config: use [category]
// - Config: comparing acceleration using bitmap 

#define uS_TO_S_FACTOR 1000000  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  5        /* Time ESP32 will go to sleep (in seconds) */

#endif