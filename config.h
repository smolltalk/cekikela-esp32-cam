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
  .awakePeriodSec = 0,
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
// - OTA
// - Add camera parameters to config

#endif