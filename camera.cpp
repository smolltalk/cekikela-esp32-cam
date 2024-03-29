#include "camera.h"

/**
 * @brief Initializes the camera sensor according to the given camera settings.
 * 
 * It must be called before calling takePicture().
 *
 * @param camera camera settings.
 *
 * @return IS_OK when it succeeds. CAMERA_INIT_ERROR in case of failure.
 *
 * @see camera_settings_t
 */
status_code_t initCamera(camera_settings_t *cameraSettings) {
  // Disable brownout detector
  // https://iotespresso.com/how-to-disable-brownout-detector-in-esp32-in-arduino/
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);

  camera_config_t config;

  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 8000000;
  config.pixel_format = PIXFORMAT_JPEG;

  config.frame_size = FRAMESIZE_XGA;
  config.jpeg_quality = 16;
  config.fb_location = CAMERA_FB_IN_PSRAM;
  config.fb_count = 2;
  config.grab_mode = CAMERA_GRAB_LATEST;

  // Init Camera
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    logError(CAMERA_LOG, "Camera init failed with error 0x%x\n", err);
    return CAMERA_INIT_ERROR;
  }

  // Camera quality adjustments
  sensor_t *s = esp_camera_sensor_get();
#ifdef LOG_LEVEL >= LOG_LEVEL_DEBUG
  logDebug(CAMERA_LOG, "Sensor default settings:\n");
  logCameraStatus(&(s->status));
#endif
  // Apply camera sensor settings from appConfig
  uint8_t sensorSettingsCount = sizeof(cameraSettings->sensor) / sizeof(sensor_param_setter_t);
  logDebug(CAMERA_LOG, "%s: sensorSettingsCount = %d.", __func__, sensorSettingsCount);
  for (uint8_t i = 0; i < sensorSettingsCount; i++) {
    sensor_param_setter_t *sensorSetting = &(cameraSettings->sensorSettingsArray[i]);
    if (sensorSetting->enabled) {
      // Apply enabled sensor setting
      logDebug(CAMERA_LOG, "%s: sensorSetting #%d.", __func__, i);
      int (**setter)(sensor_t *, int) = (int (**)(sensor_t *, int))((unsigned int)s + sensorSetting->setterOffset);
      (*setter)(s, sensorSetting->value);
    }
  }

#ifdef LOG_LEVEL >= LOG_LEVEL_INFO
  logInfo(CAMERA_LOG, "Sensor with customer settings:\n");
  logCameraStatus(&(s->status));
#endif

  // Wait until camera is ready: avoid green dark pictures.
  // Less than 1s does not work.
  delay(cameraSettings->getReadyDelayMs);
  return IS_OK;
}

/**
 * @brief Take a picture and store the data in the given frame buffer.
 *
 * The parameter cameraFrameBuffer is an input/output one.
 * Its value should be NULL before the calle and not NULL after.
 * I prefer to return the pointer to the camera_fb_t by reference rather than
 * by the return value of the function reserved here to the status code.
 *
 * @param cameraFrameBuffer pointer of pointer of camera frame buffer. Assigned by the function.
 *
 * @return IS_OK when it succeeds. CAMERA_TAKE_PICTURE_ERROR in case of failure.
 */
status_code_t takePicture(camera_fb_t **cameraFrameBuffer) {
  *cameraFrameBuffer = NULL;
  logInfo(CAMERA_LOG, "Take a picture.");
  // Take Picture with Camera
  *cameraFrameBuffer = esp_camera_fb_get();
  if (!*cameraFrameBuffer) {
    logError(CAMERA_LOG, "Failed to take a picture.");
    return CAMERA_TAKE_PICTURE_ERROR;
  }
  logInfo(CAMERA_LOG, "Done.");
  return IS_OK;
}

/**
 * @brief Free the given frame buffer and disable the lamp.
 *
 * @param cameraFrameBuffer pointer of pointer of camera frame buffer. Assigned by the function.
 */
void endCamera(camera_fb_t **cameraFrameBuffer) {
  esp_camera_fb_return(*cameraFrameBuffer);
  disableLamp();
}

/**
 * @brief Log the camera status.
 *
 * @see camera_status_t
 */
void logCameraStatus(const camera_status_t *status) {
    logInfo(CAMERA_LOG, "- framesize      = %d", status->framesize);
    logInfo(CAMERA_LOG, "- scale          = %s", bool_str(status->scale));
    logInfo(CAMERA_LOG, "- binning        = %s", bool_str(status->binning));
    logInfo(CAMERA_LOG, "- quality        = %d", status->quality);
    logInfo(CAMERA_LOG, "- brightness     = %d", status->brightness);
    logInfo(CAMERA_LOG, "- contrast       = %d", status->contrast);
    logInfo(CAMERA_LOG, "- saturation     = %d", status->saturation);
    logInfo(CAMERA_LOG, "- sharpness      = %d", status->sharpness);
    logInfo(CAMERA_LOG, "- denoise        = %d", status->denoise);
    logInfo(CAMERA_LOG, "- special_effect = %d", status->special_effect);
    logInfo(CAMERA_LOG, "- wb_mode        = %d", status->wb_mode);
    logInfo(CAMERA_LOG, "- awb            = %d", status->awb);
    logInfo(CAMERA_LOG, "- awb_gain       = %d", status->awb_gain);
    logInfo(CAMERA_LOG, "- aec            = %d", status->aec);
    logInfo(CAMERA_LOG, "- aec2           = %d", status->aec2);
    logInfo(CAMERA_LOG, "- ae_level       = %d", status->ae_level);
    logInfo(CAMERA_LOG, "- aec_value      = %d", status->aec_value);
    logInfo(CAMERA_LOG, "- agc            = %d", status->agc);
    logInfo(CAMERA_LOG, "- agc_gain       = %d", status->agc_gain);
    logInfo(CAMERA_LOG, "- gainceiling    = %d", status->gainceiling);
    logInfo(CAMERA_LOG, "- bpc            = %d", status->bpc);
    logInfo(CAMERA_LOG, "- wpc            = %d", status->wpc);
    logInfo(CAMERA_LOG, "- raw_gma        = %d", status->raw_gma);
    logInfo(CAMERA_LOG, "- lenc           = %d", status->lenc);
    logInfo(CAMERA_LOG, "- hmirror        = %d", status->hmirror);
    logInfo(CAMERA_LOG, "- vflip          = %d", status->vflip);
    logInfo(CAMERA_LOG, "- dcw            = %d", status->dcw);
    logInfo(CAMERA_LOG, "- colorbar       = %d", status->colorbar);
}

/**
 * @brief An helping function to set a value and enable a sensor_param_setter_t
 *        in a single instruction. Used by the application configuration management
 *
 * @param sensorParamSetter a pointer to a sensor_param_setter_t
 * @param value             the value to assign
 */
void setSensorSetting(sensor_param_setter_t *sensorSetting, int value) {
  sensorSetting->value = value;
  sensorSetting->enabled = true;
}

/**
 * @brief Disable the lamp to avoid flash when a photo is taken
 */
void disableLamp() {
  pinMode(LAMP_PIN, OUTPUT);
  digitalWrite(LAMP_PIN, LOW);
  rtc_gpio_hold_en(GPIO_NUM_4);
}
