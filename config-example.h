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

// TODO
// - Create config structure
// - Init a default config structure
// - Write Readme.md
// - Override config from SD file
// - Split files.mtd into upload-count.mtd and file-count.mtd
// - Make possible to take pictures periodically
// - OTA
// - DURATION_WAKEUP ? ON ISR ?
// - Read config on SD Card once

#define uS_TO_S_FACTOR 1000000  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  5        /* Time ESP32 will go to sleep (in seconds) */

#endif