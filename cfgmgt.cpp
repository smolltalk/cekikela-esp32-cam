#include "cfgmgt.h"

/**
 * Initializes the appConfig_t structure given in parameter.
 * The initialization is run only once.
 * It starts by setting default values, then custom ones
 * and finally read the configuration file when enabled.
 * If an error occurs during the configuration file reading
 * while appConfig.ignoreConfigFromSdCardReadError is false,
 * then an error statusCode is returned and the application stops there.
 * In other cases, it always returns the ok statusCode.
 *
 * @param appConfig
 * @return the initialization result, especially about the config file reading
 * 
 * @see initAppConfigWithDefaultValues()
 * @see initAppConfigWithCustomValues()
 */
statusCode initAppConfig(appConfig_t *appConfig) {
  // Setup app config once
  if (appConfig->setupConfigDone) {
    logAppConfig(appConfig);
    return ok;
  }

  // Else
  initAppConfigWithDefaultValues(appConfig);
  initAppConfigWithCustomValues(appConfig);
  appConfig->setupConfigDone = true;

  // Try to read appConfig from SD card
  statusCode result = readConfigFromSdCard(appConfig);
  logAppConfig(appConfig);
  return appConfig->ignoreConfigFromSdCardReadError ? ok : result;
}

/**
 * Set all default values of the application configuration.
 * These can be overriden by custom values in initAppConfigWithCustomValues()
 * and trough the configuration file on the SD card.
 * See the readme.md for further details about default values.
 *
 * @see initAppConfig()
 * @see initAppConfigWithCustomValues()
 */
void initAppConfigWithDefaultValues(appConfig_t *appConfig) {
  // Read the configuration from the SD card and override this
  appConfig->readConfigFromSdCard = true;
  // Save the picture on the SD card
  appConfig->savePictureOnSdCard = true;
  // Continue even if the config could not be read from the SD card
  appConfig->ignoreConfigFromSdCardReadError = true;
  // Awake Duration
  appConfig->awakeDurationMs = AWAKE_DURATION_MS_DEFAULT;
  // Awake Period
  appConfig->awakePeriodSec = AWAKE_PERIOD_SEC_DEFAULT;
  // NTP
  appConfig->syncTimePeriodHours = SYNC_TIME_PERIOD_HOURS_DEFAULT;
  // WiFi
  appConfig->wifi.enabled = false;
  // OTA
  appConfig->ota.checkPeriodHours = CHECK_PERIOD_HOURS_DEFAULT;
  appConfig->ota.url[0] = '\0';
  // Upload
  appConfig->upload.enabled = false;
  // Camera sensor settings
  appConfig->camera.getReadyDelayMs = GET_READY_DELAY_MS_DEFAULT;

  // Adjustments from https://forum.arduino.cc/t/about-esp32cam-image-too-dark-how-to-fix/1015490/5
  sensorSettings_t settingsWithInitializedSetterOffset;
  appConfig->camera.sensor = settingsWithInitializedSetterOffset;
  setSensorSetting(&(appConfig->camera.sensor.gain_ctrl), 1);      // Auto gain on
  setSensorSetting(&(appConfig->camera.sensor.exposure_ctrl), 1);  // Auto exposure on
  setSensorSetting(&(appConfig->camera.sensor.awb_gain), 1);       // Auto White Balance enable (0 or 1)
  setSensorSetting(&(appConfig->camera.sensor.brightness), 1);
  setSensorSetting(&(appConfig->camera.sensor.gainceiling), 1);
}

/**
 * Logs all attributes of the application configuration.
 * It uses the INFO log level.
 *
 * @param appConfig
 */
void logAppConfig(appConfig_t *appConfig) {
  logInfo(CFG_LOG, "Current App Config:");
  logInfo(CFG_LOG, "[root]");
  logInfo(CFG_LOG, "- setupConfigDone                 = %s", bool_str(appConfig->setupConfigDone));
  logInfo(CFG_LOG, "- readConfigFromSdCard            = %s", bool_str(appConfig->readConfigFromSdCard));
  logInfo(CFG_LOG, "- configOnSdCardRead              = %s", bool_str(appConfig->configOnSdCardRead));
  logInfo(CFG_LOG, "- ignoreConfigFromSdCardReadError = %s", bool_str(appConfig->ignoreConfigFromSdCardReadError));
  logInfo(CFG_LOG, "- savePictureOnSdCard             = %s", bool_str(appConfig->savePictureOnSdCard));
  logInfo(CFG_LOG, "- awakeDurationMs                 = %d", appConfig->awakeDurationMs);
  logInfo(CFG_LOG, "- awakePeriodSec                  = %d", appConfig->awakePeriodSec);
  logInfo(CFG_LOG, "- syncTimePeriodHours             = %d", appConfig->syncTimePeriodHours);
  logInfo(CFG_LOG, "[wifi]");
  logInfo(CFG_LOG, "- enabled                         = %s", bool_str(appConfig->wifi.enabled));
  logInfo(CFG_LOG, "- ssid                            = %s", appConfig->wifi.ssid);
  logInfo(CFG_LOG, "- password                        = %s", appConfig->wifi.password);
  logInfo(CFG_LOG, "- connectAttemptMax               = %d", appConfig->wifi.connectAttemptMax);
  logInfo(CFG_LOG, "[ota]");
  logInfo(CFG_LOG, "- checkPeriodHours                = %d", appConfig->ota.checkPeriodHours);
  logInfo(CFG_LOG, "- url                             = %s", appConfig->ota.url);
  logInfo(CFG_LOG, "[upload]");
  logInfo(CFG_LOG, "- enabled                         = %s", bool_str(appConfig->upload.enabled));
  logInfo(CFG_LOG, "- serverName                      = %s", appConfig->upload.serverName);
  logInfo(CFG_LOG, "- serverPort                      = %d", appConfig->upload.serverPort);
  logInfo(CFG_LOG, "- path                            = %s", appConfig->upload.path);
  logInfo(CFG_LOG, "- auth                            = %s", appConfig->upload.auth);
  logInfo(CFG_LOG, "- bunchSize                       = %d", appConfig->upload.bunchSize);
  logInfo(CFG_LOG, "- fileNameRandSize                = %d", appConfig->upload.fileNameRandSize);
  logInfo(CFG_LOG, "[camera]");
  logInfo(CFG_LOG, "- getReadyDelayMs                 = %d", appConfig->camera.getReadyDelayMs);
  logInfo(CFG_LOG, "- camera status will be displayed further.");
}

/**
 * Reads the configuration file on SD card and fills the
 * application configuration structure.
 * Read parameters values will override those set by 
 * initAppConfigWithDefaultValues() and by initAppConfigWithCustomValues().
 * If an error occurs during the configuration file reading
 * while appConfig.ignoreConfigFromSdCardReadError is false,
 * then an error statusCode is returned and the application stops there.
 * In other cases, it always returns the ok statusCode.
 * When appConfig.ignoreConfigFromSdCardReadError is true,
 * the process will skip all wrong written / unknown parameters.
 * 
 * @param appConfig
 * @return ok on successful reading
 *         or readConfigError on read error
 *         or sdInitError when the SD card could not be mounted
 * 
 * @see initAppConfigWithDefaultValues()
 * @see initAppConfigWithCustomValues
 */
statusCode readConfigFromSdCard(appConfig_t *appConfig) {
  statusCode statusCode = ok;

  // No requirement to read appConfig on SD Card
  // or appConfig has been already read
  if (!appConfig->readConfigFromSdCard) {
    logInfo(CFG_LOG, "Reading config on SD card is disabled.");
    return statusCode;
  }
  if (appConfig->configOnSdCardRead) {
    logInfo(CFG_LOG, "Config on SD card already read.");
    return statusCode;
  }

  logInfo(CFG_LOG, "Read config on SD card...");

  appConfig->configOnSdCardRead = true;

  // Init SD card
  statusCode = initSdCard();
  if (statusCode != ok) {
    logError(CFG_LOG, "Failed to read config on SD card!");
    return statusCode;
  }

  // Prepare all known parameters that can be read.
  paramSetter_t rootParams[] = {
    { false, "savePictureOnSdCard", &(appConfig->savePictureOnSdCard), setBool },
    { false, "awakeDurationMs", &(appConfig->awakeDurationMs), setUint16 },
    { false, "awakePeriodSec", &(appConfig->awakePeriodSec), setUint16 },
    { false, "syncTimePeriodHours", &(appConfig->syncTimePeriodHours), setUint8 },
  };

  paramSetter_t wifiParams[] = {
    { false, "enabled", &(appConfig->wifi.enabled), setBool },
    { false, "ssid", appConfig->wifi.ssid, copyCString },
    { false, "password", appConfig->wifi.password, copyEncryptedCString },
    { false, "connectAttemptMax", &(appConfig->wifi.connectAttemptMax), setUint8 }
  };

  paramSetter_t otaParams[] = {
    { false, "checkPeriodHours", &(appConfig->ota.checkPeriodHours), setUint8 },
    { false, "url", &(appConfig->ota.url), copyCString },
  };

  paramSetter_t uploadParams[] = {
    { false, "enabled", &(appConfig->upload.enabled), setBool },
    { false, "serverName", appConfig->upload.serverName, copyCString },
    { false, "serverPort", &(appConfig->upload.serverPort), setInt },
    { false, "path", appConfig->upload.path, copyCString },
    { false, "auth", appConfig->upload.auth, copyEncryptedCString },
    { false, "bunchSize", &(appConfig->upload.bunchSize), setUint8 },
    { false, "fileNameRandSize", &(appConfig->upload.fileNameRandSize), setUint8 }
  };

  paramSetter_t cameraParams[] = {
    { false, "getReadyDelayMs", &(appConfig->camera.getReadyDelayMs), setUint16 }
  };

  paramSetter_t sensorParams[] = {
    { false, "contrast", &(appConfig->camera.sensor.contrast), setCameraSensorSetting },
    { false, "brightness", &(appConfig->camera.sensor.brightness), setCameraSensorSetting },
    { false, "saturation", &(appConfig->camera.sensor.saturation), setCameraSensorSetting },
    { false, "sharpness", &(appConfig->camera.sensor.sharpness), setCameraSensorSetting },
    { false, "gainceiling", &(appConfig->camera.sensor.gainceiling), setCameraSensorSetting },
    { false, "framesize", &(appConfig->camera.sensor.framesize), setCameraSensorSetting },
    { false, "pixformat", &(appConfig->camera.sensor.pixformat), setCameraSensorSetting },
    { false, "denoise", &(appConfig->camera.sensor.denoise), setCameraSensorSetting },
    { false, "quality", &(appConfig->camera.sensor.quality), setCameraSensorSetting },
    { false, "colorbar", &(appConfig->camera.sensor.colorbar), setCameraSensorSetting },
    { false, "whitebal", &(appConfig->camera.sensor.whitebal), setCameraSensorSetting },
    { false, "gain_ctrl", &(appConfig->camera.sensor.gain_ctrl), setCameraSensorSetting },
    { false, "exposure_ctrl", &(appConfig->camera.sensor.exposure_ctrl), setCameraSensorSetting },
    { false, "hmirror", &(appConfig->camera.sensor.hmirror), setCameraSensorSetting },
    { false, "vflip", &(appConfig->camera.sensor.vflip), setCameraSensorSetting },
    { false, "aec2", &(appConfig->camera.sensor.aec2), setCameraSensorSetting },
    { false, "awb_gain", &(appConfig->camera.sensor.awb_gain), setCameraSensorSetting },
    { false, "agc_gain", &(appConfig->camera.sensor.agc_gain), setCameraSensorSetting },
    { false, "aec_value", &(appConfig->camera.sensor.aec_value), setCameraSensorSetting },
    { false, "special_effect", &(appConfig->camera.sensor.special_effect), setCameraSensorSetting },
    { false, "wb_mode", &(appConfig->camera.sensor.wb_mode), setCameraSensorSetting },
    { false, "ae_level", &(appConfig->camera.sensor.ae_level), setCameraSensorSetting },
    { false, "dcw", &(appConfig->camera.sensor.dcw), setCameraSensorSetting },
    { false, "bpc", &(appConfig->camera.sensor.bpc), setCameraSensorSetting },
    { false, "wpc", &(appConfig->camera.sensor.wpc), setCameraSensorSetting },
    { false, "raw_gma", &(appConfig->camera.sensor.raw_gma), setCameraSensorSetting },
    { false, "lenc", &(appConfig->camera.sensor.lenc), setCameraSensorSetting }
  };

  // Associate all prepared parameters array with their dedicated section
  sectionParamSetter_t sectionParams[] = {
    { "", rootParams, sizeof(rootParams) / sizeof(paramSetter_t) },
    { "wifi", wifiParams, sizeof(wifiParams) / sizeof(paramSetter_t) },
    { "ota", otaParams, sizeof(otaParams) / sizeof(paramSetter_t) },
    { "upload", uploadParams, sizeof(uploadParams) / sizeof(paramSetter_t) },
    { "camera", cameraParams, sizeof(cameraParams) / sizeof(paramSetter_t) },
    { "sensor", sensorParams, sizeof(sensorParams) / sizeof(paramSetter_t) }
  };

  fs::FS &fs = SD_MMC;
  logInfo(CFG_LOG, "File %s %s.", CONFIG_FILE_NAME, fs.exists(CONFIG_FILE_NAME) ? "exists" : "does not exist");

  FileConfig fileConfig;

  // Initialize FileConfig object
  if (fileConfig.begin(fs, CONFIG_FILE_NAME, CONFIG_VALUE_MAX_SIZE, CONFIG_VALUE_MAX_SIZE, true, appConfig->ignoreConfigFromSdCardReadError)) {
    paramSetter_t *params = sectionParams[0].params;
    size_t paramCount = sectionParams[0].paramCount;
    size_t sectionParamCount = sizeof(sectionParams) / sizeof(sectionParamSetter_t);

    logDebug(CFG_LOG, "Max param count=%d.", paramCount);

    // Loop on all parameters met in the configuration file
    while (fileConfig.readNextSetting()) {
      if (fileConfig.sectionChanged()) {
        logDebug(CFG_LOG, "Config section changed: %s.", fileConfig.getSection());
        int i = 0;
        while (i < sectionParamCount && !fileConfig.sectionIs(sectionParams[i].section)) i++;
        if (i < sectionParamCount) {
          params = sectionParams[i].params;
          paramCount = sectionParams[i].paramCount;
          logDebug(CFG_LOG, "The new section has been recognized: %s.", fileConfig.getSection());
        } else {
          params = NULL;
          paramCount = 0;
          logWarn(CFG_LOG, "The new section has not been recognized: %s.", fileConfig.getSection());
        }
      }

      // Unknown section => No params => next setting
      if (!params) {
        continue;
      }

      bool wrongParameter = true;
      logDebug(CFG_LOG, "Config current section and param name: [%s] %s.", fileConfig.getSection(), fileConfig.getName());
      for (int paramCpt = 0; paramCpt < paramCount; paramCpt++) {
        paramSetter_t *param = &(params[paramCpt]);
        if (!param->alreadySet && fileConfig.nameIs(param->paramName)) {
          param->alreadySet = true;
          logDebug(CFG_LOG, "Config call setter for param %s with value %s.", param->paramName, fileConfig.getValue());
          param->setter(&fileConfig, param->paramValueAddress);
          wrongParameter = false;
          break;
        }
      }
      if (wrongParameter) {
        logError(CFG_LOG, "Unknown or already defined parameter %s.", fileConfig.getName());
      }
    }
    fileConfig.end();
    logInfo(CFG_LOG, "Config successfully read on SD card.");
  } else {
    logError(CFG_LOG, "Failed to read config file %s!", CONFIG_FILE_NAME);
    statusCode = readConfigError;
  }

  return statusCode;
}

/**
 * Set the memory location referenced by paramValueAddress
 * with the current FileConfig parameter value casted as bool.
 *
 * @param fileConfig a reference to FileConfig handle the configuration file reading
 * @param paramValueAddress a reference to the (appConfig) parameter to set
 *
 * @see readConfigFromSdCard()
 */
void setBool(FileConfig *fileConfig, void *paramValueAddress) {
  *((bool *)paramValueAddress) = fileConfig->getBooleanValue();
}

/**
 * Set the memory location referenced by paramValueAddress
 * with the current FileConfig parameter value casted as int.
 *
 * @param fileConfig a reference to FileConfig handle the configuration file reading
 * @param paramValueAddress a reference to the (appConfig) parameter to set
 *
 * @see readConfigFromSdCard()
 */
void setInt(FileConfig *fileConfig, void *paramValueAddress) {
  *((int *)paramValueAddress) = fileConfig->getIntValue();
}

/**
 * Set the memory location referenced by paramValueAddress
 * with the current FileConfig parameter value casted as uint16_t.
 *
 * @param fileConfig a reference to FileConfig handle the configuration file reading
 * @param paramValueAddress a reference to the (appConfig) parameter to set
 *
 * @see readConfigFromSdCard()
 */
void setUint16(FileConfig *fileConfig, void *paramValueAddress) {
  *((uint16_t *)paramValueAddress) = (uint16_t)fileConfig->getIntValue();
}

/**
 * Set the memory location referenced by paramValueAddress
 * with the current FileConfig parameter value casted as uint8_t.
 *
 * @param fileConfig a reference to FileConfig handle the configuration file reading
 * @param paramValueAddress a reference to the (appConfig) parameter to set
 *
 * @see readConfigFromSdCard()
 */
void setUint8(FileConfig *fileConfig, void *paramValueAddress) {
  *((uint8_t *)paramValueAddress) = (uint8_t)fileConfig->getIntValue();
}

/**
 * Copy to the memory location referenced by paramValueAddress
 * the current FileConfig parameter value casted as a C string.
 *
 * @param fileConfig a reference to FileConfig handle the configuration file reading
 * @param paramValueAddress a reference to the (appConfig) parameter to set
 *
 * @see readConfigFromSdCard()
 */
void copyCString(FileConfig *fileConfig, void *paramValueAddress) {
  strcpy((char *)paramValueAddress, (const char *)fileConfig->getValue());
}

/**
 * Copy to the memory location referenced by paramValueAddress
 * the current FileConfig parameter decrypted value casted as a C string.
 * The parameter value is first decrypted by decryptToCString.
 * The (reverse byte array) mac address is used as the decrypting key.
 * Fill free to use an other key.
 *
 * @param fileConfig a reference to FileConfig handle the configuration file reading
 * @param paramValueAddress a reference to the (appConfig) parameter to set
 *
 * @see decryptToCString()
 * @see readConfigFromSdCard()
 */
void copyEncryptedCString(FileConfig *fileConfig, void *paramValueAddress) {
  byte key[6];
  fillReverseMacAddress(key);
  decryptToCString((char *)paramValueAddress, CONFIG_VALUE_MAX_SIZE, NULL, fileConfig->getValue(), (const char *) key, 6);
}

/**
 * Set the sensorSetting variable referenced by sensorSettingAddress
 * with the current FileConfig parameter value casted as int.
 *
 * @param fileConfig a reference to FileConfig handle the configuration file reading
 * @param sensorSettingAddress a reference to the (sensorSetting_t) parameter to set
 *
 * @see readConfigFromSdCard()
 */
void setCameraSensorSetting(FileConfig *fileConfig, void *sensorSettingAddress) {
  sensorSetting_t *sensorSetting = (sensorSetting_t *)sensorSettingAddress;
  sensorSetting->enabled = true;
  sensorSetting->value = fileConfig->getIntValue();
}

/*
 * Decrypt a base64 C string to a C string.
 * The key is byte array.
 * The cypher algorithm is just a XOR between the base64 string
 * and the key array.
 * 
 * To crypt a string, I suggest the following JavaScript code.
 * Just modify:
 * - the text with your one
 * - the key array with your one. If you want to use the mac address bytes,
 *   you can copy them from the application logs at the startup.
 *
 * var text = 'My program seems to work fine!';
 *
 * console.log(text)
 * var textArray = new TextEncoder().encode(text);
 * var keyArray = new Uint8Array([124, 245, 156, 96, 98, 236]);
 *
 * console.log(textArray.toLocaleString());
 * var encryptedTextArray = textArray.map((e, i) => e ^ keyArray[i % keyArray.length]);
 * console.log(encryptedTextArray.toLocaleString());
 *
 * var encryptedTextB64 = btoa(String.fromCharCode(...encryptedTextArray));
 * console.log(encryptedTextB64);
 *
 * @param dest the destination buffer receiving the decrypted C string
 * @param dlen the max size of the destination buffer
 * @param olen the effective length of the decrypted C string
 * @param b64Src the base64 source C string to decrypt
 * @param key the byte array key used to decrypt b64Src
 * @param keyLen the key size
 *
 * @see copyEncryptedCString()
 */
void decryptToCString(char *dest, const size_t dlen, size_t *olen, const char *b64Src, const char *key, const size_t keyLen) {
  size_t _olen;
  mbedtls_base64_decode((unsigned char *) dest, dlen, &_olen, (const unsigned char *)b64Src, strlen(b64Src));
  for (int i = 0; i < _olen; i++) {
    dest[i] = dest[i] ^ key[i % keyLen];
  }
  dest[_olen] = '\0';
  if (olen != NULL) {
    *olen = _olen;
  }
}
