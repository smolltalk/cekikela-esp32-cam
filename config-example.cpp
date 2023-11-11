#include "cfgmgt.h"

// Copy that function in config.cpp and rename it to setupAppConfig
void setupAppConfigExample(appConfig_t* appConfig) {
  // Settings
  // Camera sensor settings
  // Adjustments from https://forum.arduino.cc/t/about-esp32cam-image-too-dark-how-to-fix/1015490/5
  appConfig->camera.sensor.gain_ctrl.enabled=true;
  appConfig->camera.sensor.gain_ctrl.value=1;          // auto gain on
  appConfig->camera.sensor.exposure_ctrl.enabled=true;
  appConfig->camera.sensor.exposure_ctrl.value=1;      // auto exposure on
  appConfig->camera.sensor.awb_gain.enabled=true;
  appConfig->camera.sensor.awb_gain.value=1;           // Auto White Balance enable (0 or 1)
  appConfig->camera.sensor.brightness.enabled=1;
  appConfig->camera.sensor.brightness.value=1;
  appConfig->camera.sensor.gainceiling.enabled=true;
  appConfig->camera.sensor.gainceiling.value=1;
}