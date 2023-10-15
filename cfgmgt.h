#ifndef CFGMGT_H
#define CFGMGT_H

#include "Arduino.h"
#include "mbedtls/base64.h"
#include "error.h"
#include "SD.h"
#include "upload.h"
#include "wifimgt.h"
#include <SDConfig.h>

typedef struct {
  bool readConfigOnSdCard;
  bool configOnSdCardRead;
  bool savePictureOnSdCard;
  bool uploadPicture;
  uint16_t cameraGetReadyDurationMs;
  uint16_t awakeDurationMs;
  uint16_t awakePeriodSec;
  wifiInfo_t wifiInfo;
  uploadInfo_t uploadInfo;
} appConfig_t;

statusCode readConfigOnSDCard(appConfig_t * appConfig);

#endif