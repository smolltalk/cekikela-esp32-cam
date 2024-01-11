/**
 * Cekikela-esp32-cam is an application for the ESP32-Cam platform.
 * Triggered by a PIR or an internal timer, it takes a picture
 * and saves it on SD card and/or uploads it to a web server.
 * The application is fully configurable by instructions and by 
 * file stored on the SD card.
 * See the readme for details.
 * 
 * The project is named CeKiKeLa which means "C'est qui qu'est là ?"" i.e. "Who's there?".
 * It is hosted by the 'fab lab' MakerSpace56 (https://makerspace56.org).
 * It aims to take picture of birds (passerines) like robins and chickadees.
 *
 * Usage:
 *  - In the Arduino IDE, select Board "AI Thinker ESP32-CAM"
 *  - To enable the OTA feature, select the right partition scheme (Arduino IDE > Tool > Partition scheme):
 *    "Minimal SPIFFS (1.9MB APP with OTA /190KB SPIFFS)"
 *  - If you use a PIR, connect it to GPIO 12
 *
 *
 * Special thanks to Rui Santos
 * for its project details at https://RandomNerdTutorials.com/esp32-cam-take-photo-save-microsd-card
 * See also https://randomnerdtutorials.com/esp32-timer-wake-up-deep-sleep/
 *
 * Licensed under LGPL version 2.1
 * a version of which should have been supplied with this file.
 *
 * Github: https://github.com/smolltalk/cekikela-esp32-cam
 * Author: Sébastien Morvan (morvan.sebastien@gmail.com)
 *
 */

#include "app.h"

// Initialize default config and save the config in the RTC memory.
// Thus, the config is kept along deep sleep.
RTC_DATA_ATTR app_config_t appConfig = { .setupConfigDone = false };

/**
 * The application starts here.
 * No loop.
 * Basically, it
 * - initializes the red led and serial bus
 * - prints a startup message with the version and the Mac address
 * - disables the lamp
 * - takes a picture and saves it
 * - signals the resulting status code via the red led
 * - pauses to avoid picture burst
 * - goes sleeping, waiting for a PIR and/or timer interrupt
 */
void setup() {
  status_code_t result;
  // Switch on the red led to inform that the program is running
  pinMode(RED_LED_PIN, OUTPUT);
  // Indicate the board is awake
  switchOnRedLed();
  // Init serial
  Serial.begin(115200);
  // Print starting message
  Serial.printf("esp32-cam-sd-web-pir v%s started.\n", APP_VERSION);
  Serial.print("MAC Address: ");
  displayMacAddressHex();
  Serial.print(" ");
  displayMacAddressDecArray();
  Serial.println();
  // Writing on SD card involves disabling lamp to prevent flash lighting
  disableLamp();
  // Take and save a picture
  result = takeAndSavePicture();
  // Signal result
  signalError(result);
  // Pause to prevent picture burst
  delay(appConfig.awakeDurationMs);
  // Go to sleep
  zzzzZZZZ();
}

/**
 * Take the picture and save it:
 * - setup the application configuration (by instruction and by file on SD card when enabled)
 * - initialize the camera 
 * - take the picture
 * - synchronize the time by NTP
 * - save the picture on SD card when enabled
 * - upload the picture when enabled
 * - check for a firmware update by OTA
 *
 * @return status_code_t which is used by signalError
 */
status_code_t takeAndSavePicture() {
  status_code_t result = IS_OK;
  bool pictureSavedOnSd = false;
  camera_fb_t *fb = NULL;
  char pictureName[20];
  fileCounters_t fileCounters;

  // Setup App Config
  if ((result = initAppConfig(&appConfig)) != IS_OK) {
    return result;
  }

  wifi_settings_t *wifi = &(appConfig.wifi);
  time_settings_t *time = &(appConfig.time);
  ota_settings_t *ota = &(appConfig.ota);
  upload_settings_t *uploadSettings = &(appConfig.upload);

  // Init camera
  if ((result = initCamera(&(appConfig.camera))) != IS_OK) {
    return result;
  }

  // Take picture
  if ((result = takePicture(&fb)) != IS_OK) {
    return result;
  }

  // Sync time with NTP
  syncTime(wifi, time);

  if (appConfig.savePictureOnSdCard) {
    // Writing on SD card involves flash lighting
    disableLamp();
    if ((result = initSdCard()) == IS_OK && (result = loadOrCreateFileCounters(&fileCounters)) == IS_OK) {
      computePictureNameFromIndex(pictureName, fileCounters.pictureCounter + 1);
      if ((result = savePictureOnSdCard(pictureName, fb->buf, fb->len)) == IS_OK) {
        fileCounters.pictureCounter++;
        saveFileCounters(&fileCounters);
        pictureSavedOnSd = true;
      }
    }
  }
  if (appConfig.upload.enabled) {
    if (!pictureSavedOnSd) {
      status_code_t uploadResult;
      // Failed to saved on SD card, then try to upload.
      computePictureNameFromRandom(pictureName, uploadSettings->fileNameRandSize);
      uploadResult = uploadPicture(wifi, uploadSettings, pictureName, fb->buf, fb->len);
      // Don't override result when result already contains an error code.
      if (result == IS_OK) {
        result = uploadResult;
      }
    } else {
      // Upload a bunch of files.
      result = uploadPictureFiles(wifi, uploadSettings, &fileCounters);
    }
  }

  // Update firmware OTA
  updateFirmware(wifi, ota, APP_VERSION);

  endWifi();
  endSdCard();
  endCamera(&fb);

  return result;
}

/**
 * Prepare the deep sleep and how to be waked up.
 */
void zzzzZZZZ() {
  // Switch off the red led to inform that the program is stopped
  switchOffRedLed();
  logInfo(APP_LOG, "Going to sleep now.");
  Serial.flush();

  // Wake up by PIR, ie on up edge on pin 12
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_12, 1);

  // Wake up after a configurable duration
  if (appConfig.deepSleepDurationSec) {
    logInfo(APP_LOG, "I'll wake up in %d second(s).\n", appConfig.deepSleepDurationSec);
    esp_sleep_enable_timer_wakeup(appConfig.deepSleepDurationSec * 1000000); // us to sec factor
  }

  // Go to sleep
  esp_deep_sleep_start();
  logInfo(APP_LOG, "This will never be printed");
}

/**
  * Not used.
  */
void loop() {
}
