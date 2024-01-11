#include "cfgmgt.h"

/**
 * @brief Set the application configurations attributes with custom values.
 *        It's the place where you, customer, can set the configuration
 *        attributes programmatically.
 *        Else, you can use the configuration file on SD card.
 *        See readme.md.
 */
void initAppConfigWithCustomValues(app_config_t* appConfig) {
  // // **** General ****
    
  // // Save picture on SD card
  // appConfig->savePictureOnSdCard = true;
  // // Still awaken 5000ms before going in deep sleep mode
  // appConfig->awakeDurationMs = 5000;
  // // Do not periodically wake up the board
  // appConfig->deepSleepDurationSec = 0;

  // // **** WiFi ****
  
  // // Enable WiFi connection
  // appConfig->wifi.enabled = true;
  // // WiFi SSID
  // strcpy(appConfig->wifi.ssid, "MyWifiSSID");
  // // WiFi password
  // strcpy(appConfig->wifi.password, "MyWifiPassword");
  // // 30 connection attempts max
  // appConfig->wifi.connectAttemptMax = 30;
  
  // // **** Upload ****
  
  // // Enable pictures upload
  // appConfig->upload.enabled = true;
  // // Address of the server receiving pictures
  // strcpy(appConfig->upload.serverAddress, "myserver.picture.com");
  // // Listening TCP port of the server receiving pictures
  // appConfig->upload.serverPort = 80;
  // // Upload path of the service receiving data
  // strcpy(appConfig->upload.path, "/upload.php");
  // // Authentication to keep the upload private
  // strcpy(appConfig->upload.auth, "MyUploadPassword");
  // // Upload in packs of 10 when pcitures are stored on SD card
  // appConfig->upload.bunchSize = 10;
  // // 
  // appConfig->upload.fileNameRandSize = 5;
  
  // // **** Camera ****
  
  // appConfig->camera.getReadyDelayMs = 1500;
  
  // // **** Camera sensor ****
  
  // setSensorSetting(&(appConfig->camera.sensor.gain_ctrl), 1);      // Auto gain on
  // setSensorSetting(&(appConfig->camera.sensor.exposure_ctrl), 1);  // Auto exposure on
  // setSensorSetting(&(appConfig->camera.sensor.awb_gain), 1);       // Auto White Balance enable (0 or 1)
  // setSensorSetting(&(appConfig->camera.sensor.brightness), 1);
  // setSensorSetting(&(appConfig->camera.sensor.gainceiling), 1);
}