/*********
  MakerSpace56
  Sébastien Morvan

  Thanks to Rui Santos
  for its project details at https://RandomNerdTutorials.com/esp32-cam-take-photo-save-microsd-card
  See also https://randomnerdtutorials.com/esp32-timer-wake-up-deep-sleep/
  
  IMPORTANT!!! 
   - Select Board "AI Thinker ESP32-CAM"
   - GPIO 0 must be connected to GND to upload a sketch
   - After connecting GPIO 0 to GND, press the ESP32-CAM on-board RESET button to put your board in flashing mode
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*********/

#include "camera.h"
#include "upload.h"
#include "sd.h"
#include "error.h"
#include "filename.h"
#include "timemgt.h"
#include "wifimgt.h"
#include "cfgmgt.h"
#include "app.h"

// Initialize default config:
// - Try to read the config on SD card
// - save picture on SD Card
RTC_DATA_ATTR appConfig_t appConfig = { .setupConfigDone = false, .readConfigOnSdCard = true, .savePictureOnSdCard = true };

void setup() {
  statusCode result;
  // Switch on the red led to inform that the program is running
  pinMode(RED_LED_PIN, OUTPUT);
  // Indicate the board is awake
  switchOnRedLed();
  // Init serial
  Serial.begin(115200);
  // Writing on SD card involves flash lighting
  disableLamp();
  // Run all actions
  result = runActions();
  // Signal result
  signalError(result);
  // Wait to avoid photo galore
  delay(appConfig.awakeDurationMs);
  // Go to sleep
  zzzzZZZZ();
}

statusCode runActions() {
  statusCode result = ok;
  bool pictureSavedOnSd = false;
  camera_fb_t *fb = NULL;
  char pictureName[20];
  FilesCounters filesCounters;
  wifiSettings_t *wifi = &(appConfig.wifi);
  uploadSettings_t *uploadSettings = &(appConfig.upload);

  // Setup app config once
  if (!appConfig.setupConfigDone) {
    sensorSettings_t settingsWithInitializedSetterOffset;
    appConfig.camera.sensor = settingsWithInitializedSetterOffset; 
    setupAppConfig(&appConfig);
    appConfig.setupConfigDone = true;

    // Try to read appConfig on SD card
    readConfigOnSDCard(&appConfig);
  }

  // Init camera
  if ((result = initCamera(&(appConfig.camera))) != ok) {
    return result;
  }

  // Take picture
  if ((result = takePicture(&fb)) != ok) {
    return result;
  }

  // Sync time with NTP
  updateTime(wifi);

  if (appConfig.savePictureOnSdCard) {
    // Writing on SD card involves flash lighting
    disableLamp();
    if ((result = initSDCard()) == ok && (result = readOrCreateFilesCounters(&filesCounters)) == ok) {
      computePictureNameFromIndex(pictureName, filesCounters.pictureCounter + 1);
      if ((result = savePictureOnSDCard(pictureName, fb->buf, fb->len)) == ok) {
        filesCounters.pictureCounter++;
        writeFilesCounters(&filesCounters);
        pictureSavedOnSd = true;
      }
    }
  }
  if (appConfig.upload.enabled) {
    if (!pictureSavedOnSd) {
      statusCode uploadResult;
      // Failed to saved on SD card, then try to upload.
      computePictureNameFromRandom(pictureName, uploadSettings->fileNameRandSize);
      uploadResult = uploadPicture(wifi, uploadSettings, pictureName, fb->buf, fb->len);
      // Don't override result when result already contains an error code.
      if (result == ok) {
        result = uploadResult;
      }
    } else {
      // Upload a bunch of files.
      result = uploadPictureFiles(wifi, uploadSettings, &filesCounters);
    }
  }
  endWifi();
  endSDCard();
  endCamera(&fb);

  return result;
}

void zzzzZZZZ() {
  // Switch off the red led to inform that the program is stopped
  switchOffRedLed();
  logInfo(APP_LOG, "Going to sleep now.");
  Serial.flush();

  // Wake up on PIR, ie on up edge on pin 12
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_12, 1);

  // Wake up in TIME_TO_SLEEP seconds
  if (appConfig.awakePeriodSec) {
    logInfo(APP_LOG, "I'll wake up in %d second(s).\n", appConfig.awakePeriodSec);
    esp_sleep_enable_timer_wakeup(appConfig.awakePeriodSec * uS_TO_S_FACTOR);
  }

  // Go to sleep
  esp_deep_sleep_start();
  logInfo(APP_LOG, "This will never be printed");
}

void loop() {
}
