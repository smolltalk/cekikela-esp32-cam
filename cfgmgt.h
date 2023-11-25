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
#include "FileConfig.h"

#define CONFIG_FILE_NAME "/config.txt"
#define CONFIG_VALUE_MAX_SIZE 100

typedef struct {
  bool setupConfigDone;
  bool readConfigOnSdCard;
  bool configOnSdCardRead;
  bool savePictureOnSdCard;
  uint16_t awakeDurationMs;
  uint16_t awakePeriodSec;
  wifiSettings_t wifi;
  uploadSettings_t upload;
  cameraSettings_t camera;
} appConfig_t;

typedef struct {
  bool alreadySet;
  const char * paramName;
  void * paramAddress;
  void (*setter) (FileConfig *fileConfig, void * paramAddress);
} paramSetter_t;

typedef struct {
  char * section;
  paramSetter_t *params;
  size_t paramCount;
} sectionParamSetter_t;

statusCode readConfigOnSDCard(appConfig_t * appConfig);

#endif