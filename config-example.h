#ifndef CONFIG_H
#define CONFIG_H

#include "cfgmgt.h"

appConfig_t appConfig = {
  .readConfigOnSdCard = true,
  .configOnSdCardRead = false,
  .savePictureOnSdCard = true,
  .uploadPicture = false,
  .cameraGetReadyDurationMs = 1500,
  .awakeDurationMs = 5000,
  .awakePeriodSec = 0,
  .wifiInfo = {
    .ssid = "My Wifi SSID",
    .password = "My Wifi Password",
    .connectAttemptMax = 30
  },
  .uploadInfo = {
    .serverName = "IP address or FQDN",
    .serverPort = 80,
    .uploadPath = "/upload.php",
    .auth = "an auth string",
    .bunchSize = 2,
    .fileNameRandSize = 5
  }
};

#endif