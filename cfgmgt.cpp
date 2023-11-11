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
  *((uint8_t *)paramAddress) = (uint8_t)sdConfig->getIntValue();
}

void setUint16(SDConfig *sdConfig, void *paramAddress) {
  *((uint16_t *)paramAddress) = (uint16_t)sdConfig->getIntValue();
}

void setInt(SDConfig *sdConfig, void *paramAddress) {
  *((int *)paramAddress) = sdConfig->getIntValue();
}

void setBool(SDConfig *sdConfig, void *paramAddress) {
  *((bool *)paramAddress) = sdConfig->getBooleanValue();
}

void setCameraSensorSetting(SDConfig *sdConfig, void *sensorSettingAddress) {
  sensorSetting_t *sensorSetting = (sensorSetting_t *)sensorSettingAddress;
  sensorSetting->enabled = true;
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
    { false, "wifi.enabled", &(appConfig->wifi.enabled), setBool },
    { false, "wifi.ssid", appConfig->wifi.ssid, copyCharStr },
    { false, "wifi.password", appConfig->wifi.password, copyCharStr },
    { false, "wifi.connectAttemptMax", &(appConfig->wifi.connectAttemptMax), setUint8 },
    { false, "upload.enabled", &(appConfig->upload.enabled), setBool },
    { false, "upload.serverName", appConfig->upload.serverName, copyCharStr },
    { false, "upload.serverPort", &(appConfig->upload.serverPort), setInt },
    { false, "upload.path", appConfig->upload.path, copyCharStr },
    { false, "upload.auth", appConfig->upload.auth, copyCharStr },
    { false, "upload.bunchSize", &(appConfig->upload.bunchSize), setUint8 },
    { false, "upload.fileNameRandSize", &(appConfig->upload.fileNameRandSize), setUint8 },
    { false, "camera.getReadyDelayMs", &(appConfig->camera.getReadyDelayMs), setUint16 },
    { false, "sensor.contrast", &(appConfig->camera.sensor.contrast), setCameraSensorSetting },
    { false, "sensor.brightness", &(appConfig->camera.sensor.brightness), setCameraSensorSetting },
    { false, "sensor.saturation", &(appConfig->camera.sensor.saturation), setCameraSensorSetting },
    { false, "sensor.sharpness", &(appConfig->camera.sensor.sharpness), setCameraSensorSetting },
    { false, "sensor.gainceiling", &(appConfig->camera.sensor.gainceiling), setCameraSensorSetting },
    { false, "sensor.framesize", &(appConfig->camera.sensor.framesize), setCameraSensorSetting },
    { false, "sensor.pixformat", &(appConfig->camera.sensor.pixformat), setCameraSensorSetting },
    { false, "sensor.denoise", &(appConfig->camera.sensor.denoise), setCameraSensorSetting },
    { false, "sensor.quality", &(appConfig->camera.sensor.quality), setCameraSensorSetting },
    { false, "sensor.colorbar", &(appConfig->camera.sensor.colorbar), setCameraSensorSetting },
    { false, "sensor.whitebal", &(appConfig->camera.sensor.whitebal), setCameraSensorSetting },
    { false, "sensor.gain_ctrl", &(appConfig->camera.sensor.gain_ctrl), setCameraSensorSetting },
    { false, "sensor.exposure_ctrl", &(appConfig->camera.sensor.exposure_ctrl), setCameraSensorSetting },
    { false, "sensor.hmirror", &(appConfig->camera.sensor.hmirror), setCameraSensorSetting },
    { false, "sensor.vflip", &(appConfig->camera.sensor.vflip), setCameraSensorSetting },
    { false, "sensor.aec2", &(appConfig->camera.sensor.aec2), setCameraSensorSetting },
    { false, "sensor.awb_gain", &(appConfig->camera.sensor.awb_gain), setCameraSensorSetting },
    { false, "sensor.agc_gain", &(appConfig->camera.sensor.agc_gain), setCameraSensorSetting },
    { false, "sensor.aec_value", &(appConfig->camera.sensor.aec_value), setCameraSensorSetting },
    { false, "sensor.special_effect", &(appConfig->camera.sensor.special_effect), setCameraSensorSetting },
    { false, "sensor.wb_mode", &(appConfig->camera.sensor.wb_mode), setCameraSensorSetting },
    { false, "sensor.ae_level", &(appConfig->camera.sensor.ae_level), setCameraSensorSetting },
    { false, "sensor.dcw", &(appConfig->camera.sensor.dcw), setCameraSensorSetting },
    { false, "sensor.bpc", &(appConfig->camera.sensor.bpc), setCameraSensorSetting },
    { false, "sensor.wpc", &(appConfig->camera.sensor.wpc), setCameraSensorSetting },
    { false, "sensor.raw_gma", &(appConfig->camera.sensor.raw_gma), setCameraSensorSetting },
    { false, "sensor.lenc", &(appConfig->camera.sensor.lenc), setCameraSensorSetting }
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