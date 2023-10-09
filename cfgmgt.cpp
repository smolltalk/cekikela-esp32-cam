#include "cfgmgt.h"

void decrypt(const char * b64Input, char * key, char * output) {
  size_t keyLen = strlen(key);
  size_t encOutputLen;
  unsigned char encOutput[100];
  mbedtls_base64_decode(encOutput, 100, &encOutputLen, (const unsigned char *) b64Input, strlen(b64Input));
  for(int i = 0; i < encOutputLen; i++) {
    output[i] = encOutput[i] ^ key[i % keyLen];
  }
  encOutput[encOutputLen] = '\0';
}

statusCode readConfigOnSDCard(appConfig_t * appConfig) {
  statusCode statusCode = ok;

  // No requirement to read appConfig on SD Card
  // or appConfig has been already read
  if (!appConfig->readConfigOnSdCard || appConfig->configOnSdCardRead) {
    return statusCode;
  }

  // Init SD Card
  statusCode = initSDCard();
  if (statusCode != ok){
    return statusCode;
  }

  SDConfig sdConfig;

  //Initialize SDConfig object
  if(sdConfig.begin("config.txt", 100)) { 
    while (sdConfig.readNextSetting()) {
      if(sdConfig.nameIs("savePictureOnSdCard")){
        appConfig->savePictureOnSdCard = sdConfig.getBooleanValue();
      }else if (sdConfig.nameIs("uploadPicture")) {
        appConfig->uploadPicture = sdConfig.getBooleanValue();
      }else if(sdConfig.nameIs("cameraGetReadyDurationMs")){
          appConfig->cameraGetReadyDurationMs = sdConfig.getIntValue();
      }else if(sdConfig.nameIs("wifiInfo.ssid")){
          strcpy(appConfig->wifiInfo.ssid, sdConfig.getValue());
      }else if(sdConfig.nameIs("wifiInfo.password")){
          decrypt(sdConfig.getValue(), "MyKey", appConfig->wifiInfo.password);
      }else if(sdConfig.nameIs("wifiInfo.connectAttemptMax")){
          appConfig->wifiInfo.connectAttemptMax = sdConfig.getIntValue();
      }else if(sdConfig.nameIs("uploadInfo.serverName")){
          strcpy(appConfig->uploadInfo.serverName, sdConfig.getValue());
      }else if(sdConfig.nameIs("uploadInfo.serverPort")){
          appConfig->uploadInfo.serverPort = sdConfig.getIntValue();
      }else if(sdConfig.nameIs("uploadInfo.uploadPath")){
          strcpy(appConfig->uploadInfo.uploadPath, sdConfig.getValue());
      }else if(sdConfig.nameIs("uploadInfo.auth")){
          decrypt(sdConfig.getValue(), "MyKey", appConfig->uploadInfo.auth);
      }else if(sdConfig.nameIs("uploadInfo.bunchSize")){
          appConfig->uploadInfo.bunchSize = sdConfig.getIntValue();
      }else if(sdConfig.nameIs("uploadInfo.fileNameRandSize")){
          appConfig->uploadInfo.fileNameRandSize = sdConfig.getIntValue();
      }else{
        statusCode = readConfigError;
        Serial.printf("Failed to read config file %s.\n", sdConfig.getName());
      }
    }
  sdConfig.end();
  }

  return statusCode;
}