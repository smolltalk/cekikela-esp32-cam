#ifndef CFGMGT_H
#define CFGMGT_H

#include "Arduino.h"
#include "FileConfig.h"
#include "mbedtls/base64.h"
#include "camera.h"
#include "error.h"
#include "logging.h"
#include "ota.h"
#include "SD.h"
#include "sensor.h"
#include "upload.h"
#include "wifimgt.h"

// Logger name for this module
#define CFG_LOG "Config"

// Config file path on the SD card
#define CONFIG_FILE_NAME "/config.txt"
// Maximum size in byte of a parameter value
#define CONFIG_VALUE_MAX_SIZE 100

// Default value for the parameter appConfig_t.awakeDurationMs
#define AWAKE_DURATION_MS_DEFAULT 2000 /* 2s */
// Default value for the parameter appConfig_t.awakePeriodSec
#define AWAKE_PERIOD_SEC_DEFAULT 0

/**
 * Structure of the application configuration.
 * See the global variable appConfig in the main file.
 *
 * @see initAppConfig()
 * @see initAppConfigWithDefaultValues()
 * @see initAppConfigWithCustomValues()
*/
typedef struct {
  bool setupConfigDone;                  // Internal. False until the configuration setup in done.
  bool readConfigFromSdCard;             // User. Set it to true to read the configuration from the SD card.
  bool configOnSdCardRead;               // Internal. False until the configuration on the SD card is read.
  bool ignoreConfigFromSdCardReadError;  // User. Set it to true to ignore errors occuring during the configuration file reading.
  bool savePictureOnSdCard;              // User. Set it to true to save pictures on the SD card.
  uint16_t awakeDurationMs;              // User. Set a value in milliseconds to pause once the picture is taken to prevent picture burst.
  uint16_t awakePeriodSec;               // User. Set a value in seconds defining the deep sleep duration before the wake up. 0 means infinite.
  uint8_t syncTimePeriodHours;           // User. Set a value in hours defining the period to synchronize the time by NTP.
  wifiSettings_t wifi;                   // User. Set the WiFi settings. See wifiSettings_t.
  otaSettings_t ota;                     // User. Set the OTA settings. See otaSettings_t.
  uploadSettings_t upload;               // User. Set the picture upload settings. See uploadSettings_t.
  cameraSettings_t camera;               // User. Set the camera settings. See cameraSettings_t.
} appConfig_t;

statusCode initAppConfig(appConfig_t *appConfig);
void initAppConfigWithDefaultValues(appConfig_t *appConfig);
void initAppConfigWithCustomValues(appConfig_t *appConfig);
void logAppConfig(appConfig_t *appConfig);

/**
 * Helper structure defining one parameter that can be read in the configuration file.
 * It helps readConfigFromSdCard to fill the appConfig_t structure in the right way when
 * the parameter is met in the configuration file.
 *
 * @see readConfigFromSdCard()
 */
typedef struct {
  bool alreadySet; // False until the parameter has been read once in the configuration file
  const char *paramName; // Parameter name.
  void *paramValueAddress; // parameter value address
  void (*setter)(FileConfig *fileConfig, void *paramValueAddress);
} paramSetter_t;

/**
 * Helper structure containing an array of parameters (paramSetter_t) 
 * associated with a configuration section (ex: [wifi]).
 * It helps readConfigFromSdCard to search faster for the right paramSetter_t
 * when it meets a parameter name.
 *
 * @see readConfigFromSdCard()
 */
typedef struct {
  char *section;
  paramSetter_t *params;
  size_t paramCount;
} sectionParamSetter_t;

statusCode readConfigFromSdCard(appConfig_t *appConfig);
void setBool(FileConfig *fileConfig, void *paramValueAddress);
void setInt(FileConfig *fileConfig, void *paramValueAddress);
void setUint16(FileConfig *fileConfig, void *paramValueAddress);
void setUint8(FileConfig *fileConfig, void *paramValueAddress);
void copyCString(FileConfig *fileConfig, void *paramValueAddress);
void copyEncryptedCString(FileConfig *fileConfig, void *paramValueAddress);
void decryptToCString(char *dest, const size_t dlen, size_t *olen, const char *b64Src, const char *key, const size_t keyLen);
void setCameraSensorSetting(FileConfig *fileConfig, void *sensorSettingAddress);

#endif