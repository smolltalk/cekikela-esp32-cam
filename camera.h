#ifndef CAMERA_H
#define CAMERA_H

#include "Arduino.h"
#include "esp_camera.h"
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include "driver/rtc_io.h"
#include "error.h"

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
  void * setterOffset;
  int value;
} sensorSetting_t;

typedef struct {
  uint16_t getReadyDurationMs;
  int sensorSettingCount;
  sensorSetting_t sensorSettings[32];
} cameraSetting_t;

statusCode initCamera(cameraSetting_t * cameraSetting);
void enableLamp();
void disableLamp();
void setLamp(int newVal);
statusCode takePicture(camera_fb_t **fb);
void endCamera(camera_fb_t **fb);

#endif