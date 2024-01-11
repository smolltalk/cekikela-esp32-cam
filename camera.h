#ifndef CAMERA_H
#define CAMERA_H

#include "Arduino.h"
#include "driver/rtc_io.h"
#include "error.h"
#include "logging.h"
#include "esp_camera.h"
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"

// Logger name for this module
#define CAMERA_LOG "Camera"

// Pin definition for CAMERA_MODEL_AI_THINKER
// used to populate camera_config_t in initCamera().
// These values come from AI Thinker example projects.
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

// Lamp (Flash) PIN
#define LAMP_PIN 4
// A free PWM channel (some channels used by camera)
#define LAMP_CHANNEL 7
// Duty cycle bit range
#define PWM_RESOLUTION 9

// Default value of camera_settings_t.getReadyDelayMs
#define GET_READY_DELAY_MS_DEFAULT 1500

/**
 * Helper structure to set a value to a sensor_t parameter.
 * It is used by the configuration management
 * to override the default value for certain parameters only.
 * The parameters are the sensor_t attributes.
 * As they are all of type int, 'value' is also int type.
 *
 * @see sensor_settings_t
 * @see sensor_t
 */
typedef struct {
  bool enabled;               // true to assign the value to the parameter. False to let the default value.
  int value;                  // value to assign to the parameter.
  unsigned int setterOffset;  // sensor_t attribute position offset.
} sensor_param_setter_t;

/**
 * Sensor settings.
 * A set of optional parameters associated with sensor_t.
 * 
 * @see sensor_param_setter_t
 * @see camera_settings_t
 * @see sensor_t
 */
typedef struct {
  sensor_param_setter_t contrast = { false, 0, offsetof(sensor_t, set_contrast) };
  sensor_param_setter_t brightness = { false, 0, offsetof(sensor_t, set_brightness) };
  sensor_param_setter_t saturation = { false, 0, offsetof(sensor_t, set_saturation) };
  sensor_param_setter_t sharpness = { false, 0, offsetof(sensor_t, set_sharpness) };
  sensor_param_setter_t denoise = { false, 0, offsetof(sensor_t, set_denoise) };
  sensor_param_setter_t quality = { false, 0, offsetof(sensor_t, set_quality) };
  sensor_param_setter_t colorbar = { false, 0, offsetof(sensor_t, set_colorbar) };
  sensor_param_setter_t whitebal = { false, 0, offsetof(sensor_t, set_whitebal) };
  sensor_param_setter_t gain_ctrl = { false, 0, offsetof(sensor_t, set_gain_ctrl) };
  sensor_param_setter_t exposure_ctrl = { false, 0, offsetof(sensor_t, set_exposure_ctrl) };
  sensor_param_setter_t hmirror = { false, 0, offsetof(sensor_t, set_hmirror) };
  sensor_param_setter_t vflip = { false, 0, offsetof(sensor_t, set_vflip) };
  sensor_param_setter_t aec2 = { false, 0, offsetof(sensor_t, set_aec2) };
  sensor_param_setter_t awb_gain = { false, 0, offsetof(sensor_t, set_awb_gain) };
  sensor_param_setter_t agc_gain = { false, 0, offsetof(sensor_t, set_agc_gain) };
  sensor_param_setter_t aec_value = { false, 0, offsetof(sensor_t, set_aec_value) };
  sensor_param_setter_t special_effect = { false, 0, offsetof(sensor_t, set_special_effect) };
  sensor_param_setter_t wb_mode = { false, 0, offsetof(sensor_t, set_wb_mode) };
  sensor_param_setter_t ae_level = { false, 0, offsetof(sensor_t, set_ae_level) };
  sensor_param_setter_t dcw = { false, 0, offsetof(sensor_t, set_dcw) };
  sensor_param_setter_t bpc = { false, 0, offsetof(sensor_t, set_bpc) };
  sensor_param_setter_t wpc = { false, 0, offsetof(sensor_t, set_wpc) };
  sensor_param_setter_t raw_gma = { false, 0, offsetof(sensor_t, set_raw_gma) };
  sensor_param_setter_t lenc = { false, 0, offsetof(sensor_t, set_lenc) };
  sensor_param_setter_t gainceiling = { false, 0, offsetof(sensor_t, set_gainceiling) };
  sensor_param_setter_t framesize = { false, 0, offsetof(sensor_t, set_framesize) };
  sensor_param_setter_t pixformat = { false, 0, offsetof(sensor_t, set_pixformat) };
} sensor_settings_t;

/**
 * Camera settings.
 * Essentially composed of sensor_param_setter_t.
 * This allows to modify the value of certain sensor_t parameters
 * and to keep the default value (see sensor.h?) for other ones.
 *
 * @see sensor_settings_t
 * @see sensor_param_setter_t
 */
typedef struct {
  uint16_t getReadyDelayMs;  // Delay to let the camera sensor get ready
                             // especially about the environment light.
                             // This prevents "green" pictures.
                             // Default value is defined by GET_READY_DELAY_MS_DEFAULT.
                             // See configuration management to override this value.
  union {
    sensor_settings_t sensor;                       // Sensor settings.
    sensor_param_setter_t sensorSettingsArray[27];  // Unioned with an array to easily browse sensor parameters setters.
  };
} camera_settings_t;

/**
 * @brief Initialize the camera sensor according to the given camera settings.
 *
 * @param camera camera settings.
 *
 * @return IS_OK when it succeeds. CAMERA_INIT_ERROR in case of failure.
 *
 * @see camera_settings_t
 */
status_code_t initCamera(camera_settings_t *cameraSettings);

/**
 * @brief Take a picture and store the data in the given frame buffer.
 *
 * @param cameraFrameBuffer pointer of pointer of camera frame buffer. Assigned by the function.
 *
 * @return IS_OK when it succeeds. CAMERA_TAKE_PICTURE_ERROR in case of failure.
 */
status_code_t takePicture(camera_fb_t **cameraFrameBuffer);

/**
 * @brief Free the given frame buffer and disable the lamp.
 *
 * @param cameraFrameBuffer pointer of pointer of camera frame buffer. Assigned by the function.
 */
void endCamera(camera_fb_t **cameraFrameBuffer);

/**
 * @brief Log the camera status.
 *
 * @see camera_status_t
 */
void logCameraStatus(const camera_status_t *status);

/**
 * @brief An helping function to set a value and enable a sensor_param_setter_t
 *        in a single instruction. Used by the application configuration management
 *
 * @param sensorParamSetter a pointer to a sensor_param_setter_t
 * @param value             the value to assign
 */
void setSensorSetting(sensor_param_setter_t *sensorParamSetter, int value);


/**
 * @brief Disable the lamp to avoid flash when a photo is taken
 */
void disableLamp();

#endif