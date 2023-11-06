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

void copyCryptedCharStr(SDConfig *sdConfig, void *paramAddress, void *paramAddress2) {
  decrypt(sdConfig->getValue(), "MyKey", (char *)paramAddress);
}

void copyCharStr(SDConfig *sdConfig, void *paramAddress, void *paramAddress2) {
  strcpy((char *)paramAddress, (const char *)sdConfig->getValue());
}

void setUint8(SDConfig *sdConfig, void *paramAddress, void *paramAddress2) {
  *((uint8_t *)paramAddress) = (uint8_t) sdConfig->getIntValue();
}

void setUint16(SDConfig *sdConfig, void *paramAddress, void *paramAddress2) {
  *((uint16_t *)paramAddress) = (uint16_t) sdConfig->getIntValue();
}

void setInt(SDConfig *sdConfig, void *paramAddress, void *paramAddress2) {
  *((int *)paramAddress) = sdConfig->getIntValue();
}

void setBool(SDConfig *sdConfig, void *paramAddress, void *paramAddress2) {
  *((bool *)paramAddress) = sdConfig->getBooleanValue();
}

void setCameraSetting(SDConfig *sdConfig, void *appConfigAddress, void *paramSetterOffset) {
  appConfig_t *appConfig = (appConfig_t *)appConfigAddress;
  cameraSetting_t *cameraSetting = &(appConfig->cameraSetting);
  sensorSetting_t *cameraParamSetting = &(cameraSetting->sensorSettings[cameraSetting->sensorSettingCount++]);
  cameraParamSetting->setterOffset = paramSetterOffset;
  cameraParamSetting->value = sdConfig->getIntValue();
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
    { false, "savePictureOnSdCard", &(appConfig->savePictureOnSdCard), NULL, setBool },
    { false, "uploadPicture", &(appConfig->uploadPicture), NULL, setBool },
    { false, "awakeDurationMs", &(appConfig->awakeDurationMs), NULL, setUint16 },
    { false, "awakePeriodSec", &(appConfig->awakePeriodSec), NULL, setUint16 },
    { false, "wifiInfo.ssid", appConfig->wifiInfo.ssid, NULL, copyCharStr },
    { false, "wifiInfo.password", appConfig->wifiInfo.password, NULL, copyCharStr },
    { false, "wifiInfo.connectAttemptMax", &(appConfig->wifiInfo.connectAttemptMax), NULL, setUint8 },
    { false, "uploadInfo.serverName", appConfig->uploadInfo.serverName, NULL, copyCharStr },
    { false, "uploadInfo.serverPort", &(appConfig->uploadInfo.serverPort), NULL, setInt },
    { false, "uploadInfo.uploadPath", appConfig->uploadInfo.uploadPath, NULL, copyCharStr },
    { false, "uploadInfo.auth", appConfig->uploadInfo.auth, NULL, copyCharStr },
    { false, "uploadInfo.bunchSize", &(appConfig->uploadInfo.bunchSize), NULL, setUint8 },
    { false, "uploadInfo.fileNameRandSize", &(appConfig->uploadInfo.fileNameRandSize), NULL, setUint8 },
    { false, "camera.getReadyDurationMs", &(appConfig->cameraSetting.getReadyDurationMs), NULL, setUint16 },
    { false, "camera.contrast", appConfig, (void *)(&((sensor_t *)NULL)->set_contrast), setCameraSetting },
    { false, "camera.brightness", appConfig, (void *)(&((sensor_t *)NULL)->set_brightness), setCameraSetting },
    { false, "camera.saturation", appConfig, (void *)(&((sensor_t *)NULL)->set_saturation), setCameraSetting },
    { false, "camera.sharpness", appConfig, (void *)(&((sensor_t *)NULL)->set_sharpness), setCameraSetting },
    { false, "camera.denoise", appConfig, (void *)(&((sensor_t *)NULL)->set_denoise), setCameraSetting },
    { false, "camera.quality", appConfig, (void *)(&((sensor_t *)NULL)->set_quality), setCameraSetting },
    { false, "camera.colorbar", appConfig, (void *)(&((sensor_t *)NULL)->set_colorbar), setCameraSetting },
    { false, "camera.whitebal", appConfig, (void *)(&((sensor_t *)NULL)->set_whitebal), setCameraSetting },
    { false, "camera.gain_ctrl", appConfig, (void *)(&((sensor_t *)NULL)->set_gain_ctrl), setCameraSetting },
    { false, "camera.exposure_ctrl", appConfig, (void *)(&((sensor_t *)NULL)->set_exposure_ctrl), setCameraSetting },
    { false, "camera.hmirror", appConfig, (void *)(&((sensor_t *)NULL)->set_hmirror), setCameraSetting },
    { false, "camera.vflip", appConfig, (void *)(&((sensor_t *)NULL)->set_vflip), setCameraSetting },
    { false, "camera.aec2", appConfig, (void *)(&((sensor_t *)NULL)->set_aec2), setCameraSetting },
    { false, "camera.awb_gain", appConfig, (void *)(&((sensor_t *)NULL)->set_awb_gain), setCameraSetting },
    { false, "camera.agc_gain", appConfig, (void *)(&((sensor_t *)NULL)->set_agc_gain), setCameraSetting },
    { false, "camera.aec_value", appConfig, (void *)(&((sensor_t *)NULL)->set_aec_value), setCameraSetting },
    { false, "camera.special_effect", appConfig, (void *)(&((sensor_t *)NULL)->set_special_effect), setCameraSetting },
    { false, "camera.wb_mode", appConfig, (void *)(&((sensor_t *)NULL)->set_wb_mode), setCameraSetting },
    { false, "camera.ae_level", appConfig, (void *)(&((sensor_t *)NULL)->set_ae_level), setCameraSetting },
    { false, "camera.dcw", appConfig, (void *)(&((sensor_t *)NULL)->set_dcw), setCameraSetting },
    { false, "camera.bpc", appConfig, (void *)(&((sensor_t *)NULL)->set_bpc), setCameraSetting },
    { false, "camera.wpc", appConfig, (void *)(&((sensor_t *)NULL)->set_wpc), setCameraSetting },
    { false, "camera.raw_gma", appConfig, (void *)(&((sensor_t *)NULL)->set_raw_gma), setCameraSetting },
    { false, "camera.lenc", appConfig, (void *)(&((sensor_t *)NULL)->set_lenc), setCameraSetting }
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
          param->setter(&sdConfig, param->paramAddress, param->paramAddress2);
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