#ifndef CFGMGT_H
#define CFGMGT_H

#include "Arduino.h"
#include "mbedtls/base64.h"
#include "camera.h"
#include "error.h"
#include "SD.h"
#include "sensor.h"
#include "upload.h"
#include "wifimgt.h"
#include "SDConfig.h"

#define CONFIG_FILE_NAME "/config.txt"
#define CONFIG_VALUE_MAX_SIZE 100

typedef struct {
  bool readConfigOnSdCard;
  bool configOnSdCardRead;
  bool savePictureOnSdCard;
  bool uploadPicture;
  uint16_t awakeDurationMs;
  uint16_t awakePeriodSec;
  wifiInfo_t wifiInfo;
  uploadInfo_t uploadInfo;
  cameraSetting_t cameraSetting;
} appConfig_t;

typedef struct {
  bool alreadySet;
  const char * paramName;
  void * paramAddress;
  void * paramAddress2;
  void (*setter) (SDConfig *config, void * paramAddress, void *paramAddress2);
} paramSetter_t;

statusCode readConfigOnSDCard(appConfig_t * appConfig);

#endif