#include "camera.h"

statusCode initCamera(cameraSettings_t *camera) {
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
    Serial.printf("Camera init failed with error 0x%x\n", err);
    return cameraInitError;
  }

  // Camera quality adjustments
  sensor_t *s = esp_camera_sensor_get();

  // Apply camera sensor settings from appConfig
  uint8_t sensorSettingsCount = sizeof(camera->sensor) / sizeof(sensorSetting_t);
  for (int i = 0; i < sensorSettingsCount; i++) {
    sensorSetting_t * sensorSetting = &(camera->sensorSettingsArray[i]);
    if (!sensorSetting->enabled) {
      continue;
    }
    // Apply enabled sensor setting 
    int (**setter)(sensor_t *, int) = (int (**)(sensor_t *, int))((unsigned int)s + sensorSetting->setterOffset);
    (*setter)(s, sensorSetting->value);
  }

  // Wait until camera is ready: avoid green dark pictures.
  // Less than 1s does not work.
  delay(camera->getReadyDelayMs);
  return ok;
}

void enableLamp() {
  rtc_gpio_hold_dis(GPIO_NUM_4);
  pinMode(LAMP_PIN, OUTPUT);
  digitalWrite(LAMP_PIN, LOW);
}

void disableLamp() {
  pinMode(LAMP_PIN, OUTPUT);
  digitalWrite(LAMP_PIN, LOW);
  rtc_gpio_hold_en(GPIO_NUM_4);
}


// Lamp Control
void setLamp(int newVal) {
#if defined(LAMP_PIN)
  if (newVal != -1) {
    // Apply a logarithmic function to the scale.
    int pwmMax = pow(2, PWM_RESOLUTION) - 1;
    int brightness = round((pow(2, (1 + (newVal * 0.02))) - 2) / 6 * pwmMax);
    ledcWrite(LAMP_CHANNEL, brightness);
    Serial.print("Lamp: ");
    Serial.print(newVal);
    Serial.print("%, pwm = ");
    Serial.println(brightness);
  }
#endif
}

statusCode takePicture(camera_fb_t **fb) {
  *fb = NULL;
  Serial.print("Take a picture...");
  // Take Picture with Camera
  *fb = esp_camera_fb_get();
  if (!*fb) {
    Serial.println(" Failed.");
    return cameraTakePictureError;
  }
  Serial.println(" Done.");
  return ok;
}

void endCamera(camera_fb_t **fb) {
  esp_camera_fb_return(*fb);
  disableLamp();
}
