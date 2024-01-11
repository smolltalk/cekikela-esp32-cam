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
#define CFG_CONFIG_FILE_NAME "/config.txt"
// Maximum size in byte of a parameter value
#define CFG_CONFIG_VALUE_MAX_SIZE 100

// Default value for the parameter app_config_t.awakeDurationMs
#define AWAKE_DURATION_MS_DEFAULT 2000 /* 2s */
// Default value for the parameter app_config_t.deepSleepDurationSec
#define DEEP_SLEEP_DURATION_SEC_DEFAULT 0

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
  uint16_t deepSleepDurationSec;         // User. Set a value in seconds defining the deep sleep duration before the wake up. 0 means infinite.
  wifi_settings_t wifi;                  // User. Set the WiFi settings. See wifi_settings_t.
  time_settings_t time;                  // User. Set the time settings like the NTP server address. See time_settings_t.
  ota_settings_t ota;                    // User. Set the OTA settings. See ota_settings_t.
  upload_settings_t upload;              // User. Set the picture upload settings. See upload_settings_t.
  camera_settings_t camera;              // User. Set the camera settings. See camera_settings_t.
} app_config_t;

/**
 * @brief Initialize the app_config_t structure given in parameter.
 *
 * @param appConfig
 *
 * @return the initialization result, especially about the config file reading
 *
 */
status_code_t initAppConfig(app_config_t *appConfig);

/**
 * @brief Set all default values of the application configuration.
 */
void initAppConfigWithDefaultValues(app_config_t *appConfig);

/**
 * @brief Set the application configurations attributes with custom values.
 *        This function is implemented in config.cpp.
 */
void initAppConfigWithCustomValues(app_config_t *appConfig);

/**
 * @brief Log all the attributes of the application configuration.
 * It uses the INFO log level.
 *
 * @param appConfig
 */
void logAppConfig(app_config_t *appConfig);

/**
 * Helper structure defining one parameter that can be read in the configuration file.
 * It helps readConfigFromSdCard to fill the app_config_t structure in the right way when
 * the parameter is met in the configuration file.
 *
 * @see readConfigFromSdCard()
 */
typedef struct paramSetter_t {
  bool alreadySet;                                                    // False until the parameter has been read once in the configuration file
  const char *paramName;                                              // Parameter name.
  void *paramValueAddress;                                            // Parameter value address
  void (*setter)(FileConfig *fileConfig, paramSetter_t *paramSetter); // Reference to the function assigning the value
  uint8_t maxSize;                                                    // Value max size: used to avoid memory overflow with C strings
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
} section_param_setter_t;

/**
 * @brief Read the configuration file on SD card and fills the
 *        application configuration structure.
 * 
 * @param appConfig
 * @return IS_OK on successful reading
 *         or READ_CONFIG_ERROR on read error
 *         or SD_INIT_ERROR when the SD card could not be mounted
 */
status_code_t readConfigFromSdCard(app_config_t *appConfig);

/**
 * @brief Set the memory location referenced by paramValueAddress
 *        with the current FileConfig parameter value casted as bool.
 *
 * @param fileConfig  a reference to FileConfig handle the configuration file reading
 * @param paramSetter a reference to parameter setter structure
 *                    containing the (app_config_t) parameter address
 *
 * @see readConfigFromSdCard()
 */
void setBool(FileConfig *fileConfig, paramSetter_t *paramSetter);

/**
 * @brief Set the memory location referenced by paramValueAddress
 *        with the current FileConfig parameter value casted as int.
 *
 * @param fileConfig  a reference to FileConfig handle the configuration file reading
 * @param paramSetter a reference to parameter setter structure
 *                    containing the (app_config_t) parameter address
 *
 * @see readConfigFromSdCard()
 */
void setInt(FileConfig *fileConfig, paramSetter_t *paramSetter);

/**
 * @brief Set the memory location referenced by paramValueAddress
 *        with the current FileConfig parameter value casted as long.
 *
 * @param fileConfig  a reference to FileConfig handle the configuration file reading
 * @param paramSetter a reference to parameter setter structure
 *                    containing the (app_config_t) parameter address
 *
 * @see readConfigFromSdCard()
 */
void setLong(FileConfig *fileConfig, paramSetter_t *paramSetter);

/**
 * @brief Set the memory location referenced by paramValueAddress
 *        with the current FileConfig parameter value casted as uint16_t.
 *
 * @param fileConfig  a reference to FileConfig handle the configuration file reading
 * @param paramSetter a reference to parameter setter structure
 *                    containing the (app_config_t) parameter address
 *
 * @see readConfigFromSdCard()
 */
void setUint16(FileConfig *fileConfig, paramSetter_t *paramSetter);

/**
 * @brief Set the memory location referenced by paramValueAddress
 *        with the current FileConfig parameter value casted as uint8_t.
 *
 * @param fileConfig  a reference to FileConfig handle the configuration file reading
 * @param paramSetter a reference to parameter setter structure
 *                    containing the (app_config_t) parameter address
 *
 * @see readConfigFromSdCard()
 */
void setUint8(FileConfig *fileConfig, paramSetter_t *paramSetter);

/**
 * @brief Copy to the memory location referenced by paramValueAddress
 *        the current FileConfig parameter value casted as a C string.
 *
 * @param fileConfig  a reference to FileConfig handle the configuration file reading
 * @param paramSetter a reference to parameter setter structure
 *                    containing the (app_config_t) parameter address
 *                    and the C string max size
 *
 * @see readConfigFromSdCard()
 */
void copyCString(FileConfig *fileConfig, paramSetter_t *paramSetter);

/**
 * @brief Copy to the memory location referenced by paramValueAddress
 *        the current FileConfig parameter decrypted value casted as a C string.
 *
 * @param fileConfig  a reference to FileConfig handle the configuration file reading
 * @param paramSetter a reference to parameter setter structure
 *                    containing the (app_config_t) parameter address
 *                    and the C string max size
 *
 * @see decryptToCString()
 * @see readConfigFromSdCard()
 */
void copyEncryptedCString(FileConfig *fileConfig, paramSetter_t *paramSetter);

/**
 * @brief Set the sensorSetting variable referenced by sensorSettingAddress
 *        with the current FileConfig parameter value casted as int.
 *
 * @param fileConfig  a reference to FileConfig handle the configuration file reading
 * @param paramSetter a reference to parameter setter structure
 *                    containing the reference to the sensor parameter (sensor_param_setter_t) to set.
 *
 * @see readConfigFromSdCard()
 */
void setCameraSensorSetting(FileConfig *fileConfig, paramSetter_t *paramSetter);

/**
 * @brief Decrypt a base64 C string to a C string.
 *
 * @param dest the destination buffer receiving the decrypted C string.
 * @param dlen the max size of the destination buffer.
 * @param olen the effective length of the decrypted C string.
 * @param b64Src the base64 source C string to decrypt.
 * @param key the byte array key used to decrypt b64Src.
 * @param keyLen the key size.
 *
 * @see copyEncryptedCString()
 */
void decryptToCString(char *dest, const size_t dlen, size_t *olen, const char *b64Src, const char *key, const size_t keyLen);

#endif