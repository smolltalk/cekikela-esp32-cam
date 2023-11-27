#ifndef CAMERA_H
#define CAMERA_H

#include "Arduino.h"
#include "driver/rtc_io.h"
#include "error.h"
#include "logging.h"
#include "esp_camera.h"
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"

#define CAMERA_LOG "Camera"

// Pin definition for CAMERA_MODEL_AI_THINKER
#define PWDN_GPIO_NUM 32
#define RESET_GPIO_NUM -1
#define XCLK_GPIO_NUM 0
#define SIOD_GPIO_NUM 26
#define SIOC_GPIO_NUM 27

#define Y9_GPIO_NUM 35
#define Y8_GPIO_NUM 34
#define Y7_GPIO_NUM 39
#define Y6_GPIO_NUM 36
#define Y5_GPIO_NUM 21
#define Y4_GPIO_NUM 19
#define Y3_GPIO_NUM 18
#define Y2_GPIO_NUM 5
#define VSYNC_GPIO_NUM 25
#define HREF_GPIO_NUM 23
#define PCLK_GPIO_NUM 22

#define LAMP_PIN 4
// a free PWM channel (some channels used by camera)
#define LAMP_CHANNEL 7
// 50K pwm frequency
#define PWM_FREQ 50000
// duty cycle bit range
#define PWM_RESOLUTION 9

typedef struct {
  bool enabled;
  int value;
  unsigned int setterOffset;
} sensorSetting_t;

typedef struct {
  sensorSetting_t contrast = { false, 0, offsetof(sensor_t, set_contrast) };
  sensorSetting_t brightness = { false, 0, offsetof(sensor_t, set_brightness) };
  sensorSetting_t saturation = { false, 0, offsetof(sensor_t, set_saturation) };
  sensorSetting_t sharpness = { false, 0, offsetof(sensor_t, set_sharpness) };
  sensorSetting_t denoise = { false, 0, offsetof(sensor_t, set_denoise) };
  sensorSetting_t quality = { false, 0, offsetof(sensor_t, set_quality) };
  sensorSetting_t colorbar = { false, 0, offsetof(sensor_t, set_colorbar) };
  sensorSetting_t whitebal = { false, 0, offsetof(sensor_t, set_whitebal) };
  sensorSetting_t gain_ctrl = { false, 0, offsetof(sensor_t, set_gain_ctrl) };
  sensorSetting_t exposure_ctrl = { false, 0, offsetof(sensor_t, set_exposure_ctrl) };
  sensorSetting_t hmirror = { false, 0, offsetof(sensor_t, set_hmirror) };
  sensorSetting_t vflip = { false, 0, offsetof(sensor_t, set_vflip) };
  sensorSetting_t aec2 = { false, 0, offsetof(sensor_t, set_aec2) };
  sensorSetting_t awb_gain = { false, 0, offsetof(sensor_t, set_awb_gain) };
  sensorSetting_t agc_gain = { false, 0, offsetof(sensor_t, set_agc_gain) };
  sensorSetting_t aec_value = { false, 0, offsetof(sensor_t, set_aec_value) };
  sensorSetting_t special_effect = { false, 0, offsetof(sensor_t, set_special_effect) };
  sensorSetting_t wb_mode = { false, 0, offsetof(sensor_t, set_wb_mode) };
  sensorSetting_t ae_level = { false, 0, offsetof(sensor_t, set_ae_level) };
  sensorSetting_t dcw = { false, 0, offsetof(sensor_t, set_dcw) };
  sensorSetting_t bpc = { false, 0, offsetof(sensor_t, set_bpc) };
  sensorSetting_t wpc = { false, 0, offsetof(sensor_t, set_wpc) };
  sensorSetting_t raw_gma = { false, 0, offsetof(sensor_t, set_raw_gma) };
  sensorSetting_t lenc = { false, 0, offsetof(sensor_t, set_lenc) };
  sensorSetting_t gainceiling = { false, 0, offsetof(sensor_t, set_gainceiling) };
  sensorSetting_t framesize = { false, 0, offsetof(sensor_t, set_framesize) };
  sensorSetting_t pixformat = { false, 0, offsetof(sensor_t, set_pixformat) };
} sensorSettings_t;

typedef struct {
  uint16_t getReadyDelayMs;
  union {
    sensorSettings_t sensor;
    sensorSetting_t sensorSettingsArray[27];
  };
} cameraSettings_t;

statusCode initCamera(cameraSettings_t *camera);
void enableLamp();
void disableLamp();
void setLamp(int newVal);
statusCode takePicture(camera_fb_t **fb);
void endCamera(camera_fb_t **fb);
void setSensorSetting(sensorSetting_t *sensorSetting, int value);
void displayCameraStatus(const camera_status_t *status);

#endif