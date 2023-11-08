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

void copyCryptedCharStr(SDConfig *sdConfig, void *paramAddress) {
  decrypt(sdConfig->getValue(), "MyKey", (char *)paramAddress);
}

void copyCharStr(SDConfig *sdConfig, void *paramAddress) {
  strcpy((char *)paramAddress, (const char *)sdConfig->getValue());
}

void setUint8(SDConfig *sdConfig, void *paramAddress) {
  *((uint8_t *)paramAddress) = (uint8_t) sdConfig->getIntValue();
}

void setUint16(SDConfig *sdConfig, void *paramAddress) {
  *((uint16_t *)paramAddress) = (uint16_t) sdConfig->getIntValue();
}

void setInt(SDConfig *sdConfig, void *paramAddress) {
  *((int *)paramAddress) = sdConfig->getIntValue();
}

void setBool(SDConfig *sdConfig, void *paramAddress) {
  *((bool *)paramAddress) = sdConfig->getBooleanValue();
}

void setCameraSensorSetting(SDConfig *sdConfig, void *sensorSettingddress) {
  sensorSetting_t *sensorSetting = (sensorSetting_t *) sensorSettingddress;
  sensorSetting->enabled= true;
  sensorSetting->value = sdConfig->getIntValue();
}

statusCode readConfigOnSDCard(appConfig_t *appConfig) {
  statusCode statusCode = ok;

  // No requirement to read appConfig on SD Card
  // or appConfig has been already read
  if (!appConfig->readConfigOnSdCard) {
    Serial.println("Do not read config on SD card.");
    return statusCode;
  }
  if (appConfig->configOnSdCardRead) {
    Serial.println("Config on SD card already read.");
    return statusCode;
  }

  Serial.println("Read config on SD card...");

  appConfig->configOnSdCardRead = true;

  // Init SD Card
  statusCode = initSDCard();
  if (statusCode != ok) {
    Serial.println("Failed to read config on SD card!");
    return statusCode;
  }

  paramSetter_t params[] = {
    { false, "savePictureOnSdCard", &(appConfig->savePictureOnSdCard), setBool },
    { false, "awakeDurationMs", &(appConfig->awakeDurationMs), setUint16 },
    { false, "awakePeriodSec", &(appConfig->awakePeriodSec), setUint16 },
    { false, "wifi.enabled", &(appConfig->wifiSettings.enabled), setBool },
    { false, "wifi.ssid", appConfig->wifiSettings.ssid, copyCharStr },
    { false, "wifi.password", appConfig->wifiSettings.password, copyCharStr },
    { false, "wifi.connectAttemptMax", &(appConfig->wifiSettings.connectAttemptMax), setUint8 },
    { false, "upload.enabled", &(appConfig->uploadSettings.enabled), setBool },
    { false, "upload.serverName", appConfig->uploadSettings.serverName, copyCharStr },
    { false, "upload.serverPort", &(appConfig->uploadSettings.serverPort), setInt },
    { false, "upload.uploadPath", appConfig->uploadSettings.uploadPath, copyCharStr },
    { false, "upload.auth", appConfig->uploadSettings.auth, copyCharStr },
    { false, "upload.bunchSize", &(appConfig->uploadSettings.bunchSize), setUint8 },
    { false, "upload.fileNameRandSize", &(appConfig->uploadSettings.fileNameRandSize), setUint8 },
    { false, "camera.getReadyDurationMs", &(appConfig->cameraSettings.getReadyDurationMs), setUint16 },
    { false, "camera.contrast", &(appConfig->cameraSettings.sensorSettings.contrast), setCameraSensorSetting },
    { false, "camera.brightness", &(appConfig->cameraSettings.sensorSettings.brightness), setCameraSensorSetting },
    { false, "camera.saturation", &(appConfig->cameraSettings.sensorSettings.saturation), setCameraSensorSetting },
    { false, "camera.sharpness", &(appConfig->cameraSettings.sensorSettings.sharpness), setCameraSensorSetting },
    { false, "camera.denoise", &(appConfig->cameraSettings.sensorSettings.denoise), setCameraSensorSetting },
    { false, "camera.quality", &(appConfig->cameraSettings.sensorSettings.quality), setCameraSensorSetting },
    { false, "camera.colorbar", &(appConfig->cameraSettings.sensorSettings.colorbar), setCameraSensorSetting },
    { false, "camera.whitebal", &(appConfig->cameraSettings.sensorSettings.whitebal), setCameraSensorSetting },
    { false, "camera.gain_ctrl", &(appConfig->cameraSettings.sensorSettings.gain_ctrl), setCameraSensorSetting },
    { false, "camera.exposure_ctrl", &(appConfig->cameraSettings.sensorSettings.exposure_ctrl), setCameraSensorSetting },
    { false, "camera.hmirror", &(appConfig->cameraSettings.sensorSettings.hmirror), setCameraSensorSetting },
    { false, "camera.vflip", &(appConfig->cameraSettings.sensorSettings.vflip), setCameraSensorSetting },
    { false, "camera.aec2", &(appConfig->cameraSettings.sensorSettings.aec2), setCameraSensorSetting },
    { false, "camera.awb_gain", &(appConfig->cameraSettings.sensorSettings.awb_gain), setCameraSensorSetting },
    { false, "camera.agc_gain", &(appConfig->cameraSettings.sensorSettings.agc_gain), setCameraSensorSetting },
    { false, "camera.aec_value", &(appConfig->cameraSettings.sensorSettings.aec_value), setCameraSensorSetting },
    { false, "camera.special_effect", &(appConfig->cameraSettings.sensorSettings.special_effect), setCameraSensorSetting },
    { false, "camera.wb_mode", &(appConfig->cameraSettings.sensorSettings.wb_mode), setCameraSensorSetting },
    { false, "camera.ae_level", &(appConfig->cameraSettings.sensorSettings.ae_level), setCameraSensorSetting },
    { false, "camera.dcw", &(appConfig->cameraSettings.sensorSettings.dcw), setCameraSensorSetting },
    { false, "camera.bpc", &(appConfig->cameraSettings.sensorSettings.bpc), setCameraSensorSetting },
    { false, "camera.wpc", &(appConfig->cameraSettings.sensorSettings.wpc), setCameraSensorSetting },
    { false, "camera.raw_gma", &(appConfig->cameraSettings.sensorSettings.raw_gma), setCameraSensorSetting },
    { false, "camera.lenc", &(appConfig->cameraSettings.sensorSettings.lenc), setCameraSensorSetting }
  };

  fs::FS &fs = SD_MMC;
  Serial.printf("File %s %s.\n", CONFIG_FILE_NAME, fs.exists(CONFIG_FILE_NAME) ? "exists" : "does not exist");

  SDConfig sdConfig;

  //Initialize SDConfig object
  if (sdConfig.begin(fs, CONFIG_FILE_NAME, CONFIG_VALUE_MAX_SIZE)) {
    size_t paramCount = sizeof(params) / sizeof(paramSetter_t);
    Serial.printf("Max param count=%d\n", paramCount);
    while (sdConfig.readNextSetting()) {
      bool wrongParameter = true;
      Serial.printf("Config current param name: %s.\n", sdConfig.getName());
      for (int paramCpt = 0; paramCpt < paramCount; paramCpt++) {
        paramSetter_t *param = &(params[paramCpt]);
        if (!param->alreadySet && sdConfig.nameIs(param->paramName)) {
          param->alreadySet = true;
          Serial.printf("Config call setter for param %s with value %s.\n", param->paramName, sdConfig.getValue());
          param->setter(&sdConfig, param->paramAddress);
          wrongParameter = false;
          break;
        }
      }
      if (wrongParameter) {
        Serial.printf("Unknown or already defined parameter %s.\n", sdConfig.getName());
      }
    }
    sdConfig.end();
    Serial.printf("Config successfully read on SD card.");
  } else {
    Serial.printf("Failed to read config file %s!\n", CONFIG_FILE_NAME);
    statusCode = readConfigError;
  }

  return statusCode;
}