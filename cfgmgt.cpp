#include "cfgmgt.h"

void decrypt(const char *b64Input, char *key, char *output) {
  size_t keyLen = strlen(key);
  size_t encOutputLen;
  unsigned char encOutput[100];
  mbedtls_base64_decode(encOutput, 100, &encOutputLen, (const unsigned char *)b64Input, strlen(b64Input));
  for (int i = 0; i < encOutputLen; i++) {
    output[i] = encOutput[i] ^ key[i % keyLen];
  }
  encOutput[encOutputLen] = '\0';
}

void copyCryptedCharStr(FileConfig *fileConfig, void *paramAddress) {
  decrypt(fileConfig->getValue(), "MyKey", (char *)paramAddress);
}

void copyCharStr(FileConfig *fileConfig, void *paramAddress) {
  strcpy((char *)paramAddress, (const char *)fileConfig->getValue());
}

void setUint8(FileConfig *fileConfig, void *paramAddress) {
  *((uint8_t *)paramAddress) = (uint8_t)fileConfig->getIntValue();
}

void setUint16(FileConfig *fileConfig, void *paramAddress) {
  *((uint16_t *)paramAddress) = (uint16_t)fileConfig->getIntValue();
}

void setInt(FileConfig *fileConfig, void *paramAddress) {
  *((int *)paramAddress) = fileConfig->getIntValue();
}

void setBool(FileConfig *fileConfig, void *paramAddress) {
  *((bool *)paramAddress) = fileConfig->getBooleanValue();
}

void setCameraSensorSetting(FileConfig *fileConfig, void *sensorSettingAddress) {
  sensorSetting_t *sensorSetting = (sensorSetting_t *)sensorSettingAddress;
  sensorSetting->enabled = true;
  sensorSetting->value = fileConfig->getIntValue();
}

statusCode readConfigOnSDCard(appConfig_t *appConfig) {
  statusCode statusCode = ok;

  // No requirement to read appConfig on SD Card
  // or appConfig has been already read
  if (!appConfig->readConfigOnSdCard) {
    logInfo(CFG_LOG, "Do not read config on SD card.");
    return statusCode;
  }
  if (appConfig->configOnSdCardRead) {
    logInfo(CFG_LOG, "Config on SD card already read.");
    return statusCode;
  }

  logInfo(CFG_LOG, "Read config on SD card...");

  appConfig->configOnSdCardRead = true;

  // Init SD Card
  statusCode = initSDCard();
  if (statusCode != ok) {
    logError(CFG_LOG, "Failed to read config on SD card!");
    return statusCode;
  }

  paramSetter_t rootParams[] = {
    { false, "savePictureOnSdCard", &(appConfig->savePictureOnSdCard), setBool },
    { false, "awakeDurationMs", &(appConfig->awakeDurationMs), setUint16 },
    { false, "awakePeriodSec", &(appConfig->awakePeriodSec), setUint16 }
  };

  paramSetter_t wifiParams[] = {
    { false, "enabled", &(appConfig->wifi.enabled), setBool },
    { false, "ssid", appConfig->wifi.ssid, copyCharStr },
    { false, "password", appConfig->wifi.password, copyCharStr },
    { false, "connectAttemptMax", &(appConfig->wifi.connectAttemptMax), setUint8 }
  };

  paramSetter_t uploadParams[] = {
    { false, "enabled", &(appConfig->upload.enabled), setBool },
    { false, "serverName", appConfig->upload.serverName, copyCharStr },
    { false, "serverPort", &(appConfig->upload.serverPort), setInt },
    { false, "path", appConfig->upload.path, copyCharStr },
    { false, "auth", appConfig->upload.auth, copyCharStr },
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

  sectionParamSetter_t sectionParams[] = {
    { "", rootParams, sizeof(rootParams) / sizeof(paramSetter_t) },
    { "wifi", wifiParams, sizeof(wifiParams) / sizeof(paramSetter_t) },
    { "upload", uploadParams, sizeof(uploadParams) / sizeof(paramSetter_t) },
    { "camera", cameraParams, sizeof(cameraParams) / sizeof(paramSetter_t) },
    { "sensor", sensorParams, sizeof(sensorParams) / sizeof(paramSetter_t) }
  };

  fs::FS &fs = SD_MMC;
  logInfo(CFG_LOG, "File %s %s.", CONFIG_FILE_NAME, fs.exists(CONFIG_FILE_NAME) ? "exists" : "does not exist");

  FileConfig fileConfig;

  //Initialize FileConfig object
  if (fileConfig.begin(fs, CONFIG_FILE_NAME, CONFIG_VALUE_MAX_SIZE, CONFIG_VALUE_MAX_SIZE, true, true)) {
    paramSetter_t *params = sectionParams[0].params;
    size_t paramCount = sectionParams[0].paramCount;
    size_t sectionParamCount = sizeof(sectionParams) / sizeof(sectionParamSetter_t);

    logDebug(CFG_LOG, "Max param count=%d.", paramCount);
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
          param->setter(&fileConfig, param->paramAddress);
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
    logError(CFG_LOG, "Failed to read config file %s!\n", CONFIG_FILE_NAME);
    statusCode = readConfigError;
  }

  return statusCode;
}