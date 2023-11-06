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
#include "config.h"

#define uS_TO_S_FACTOR 1000000  /* Conversion factor for micro seconds to seconds */

void zzzzZZZZ() {
  // Switch off the red led to inform that the program is stopped
  switchOffRedLed();
  Serial.println("Going to sleep now.");
  Serial.flush();

  // Wake up on PIR, ie on up edge on pin 12
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_12, 1);
  
  // Wake up in TIME_TO_SLEEP seconds
  if (appConfig.awakePeriodSec) {
    Serial.printf("I'll wake up in %d second(s).\n")
    esp_sleep_enable_timer_wakeup(appConfig.awakePeriodSec * uS_TO_S_FACTOR);
  }

  // Go to sleep
  esp_deep_sleep_start();
  Serial.println("This will never be printed");
}

statusCode runActions() {
  statusCode result = ok;
  bool pictureSavedOnSd = false;
  camera_fb_t *fb = NULL;
  char pictureName[20];
  FilesCounters filesCounters;
  wifiInfo_t * wifiInfo = &(appConfig.wifiInfo);
  uploadInfo_t * uploadInfo = &(appConfig.uploadInfo);
  
  // Init camera
  if ((result = initCamera(&(appConfig.cameraSetting))) != ok) {
    return result;
  }

  // Take picture
  if ((result = takePicture(&fb)) != ok) {
    return result;
  }

  // Try to read appConfig on SD card
  readConfigOnSDCard(&appConfig);

  // Sync time with NTP
  updateTime(wifiInfo);

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
  if (appConfig.uploadPicture) {
    if (!pictureSavedOnSd) {
      statusCode uploadResult;
      // Failed to saved on SD card, then try to upload.
      computePictureNameFromRandom(pictureName, uploadInfo->fileNameRandSize);
      uploadResult = uploadPicture(wifiInfo, uploadInfo, pictureName, fb->buf, fb->len);
      // Don't override result when result already contains an error code.
      if (result == ok) {
        result = uploadResult;
      }
    } else {
      // Upload a bunch of files.
      result = uploadPictureFiles(wifiInfo, uploadInfo, &filesCounters);
    }
  }
  endWifi();
  endSDCard();
  endCamera(&fb);

  return result;
}

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

void loop() {
}
