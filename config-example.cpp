#include "cfgmgt.h"

// Copy that function in config.cpp and rename it to setupAppConfig
void setupAppConfigExample(appConfig_t* appConfig) {
  // Camera sensor settings
  // Adjustments from https://forum.arduino.cc/t/about-esp32cam-image-too-dark-how-to-fix/1015490/5
  setSensorSetting(&(appConfig->camera.sensor.gain_ctrl), 1);      // Auto gain on
  setSensorSetting(&(appConfig->camera.sensor.exposure_ctrl), 1);  // Auto exposure on
  setSensorSetting(&(appConfig->camera.sensor.awb_gain), 1);       // Auto White Balance enable (0 or 1)
  setSensorSetting(&(appConfig->camera.sensor.brightness), 1);
  setSensorSetting(&(appConfig->camera.sensor.gainceiling), 1);
}