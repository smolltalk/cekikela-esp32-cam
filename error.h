#ifndef ERROR_H
#define ERROR_H

#include "Arduino.h"
#include "logging.h"

// Logger name for this module
#define ERROR_LOG "Error"

// Built-in red LED pin number
#define RED_LED_PIN 33

/**
 * Status code.
 * It's used to know where the application failed.
 * signalError() causes the built-in LED to flash 
 * as many times as the numerical value associated 
 * with the status code.
 * 
 * @see signalError()
 */
typedef enum {
  IS_OK = 0,                     // Sucessful
  CAMERA_INIT_ERROR = 1,         // Failed to detect or initialize the camera sensor
  CAMERA_TAKE_PICTURE_ERROR = 2, // Failed to take a picture
  SD_INIT_ERROR = 3,             // Failed to detect or mount the SD card
  SD_READ_ERROR = 4,             // Failed to read a file on the SD card
  SD_WRITE_ERROR = 5,            // Failed to write a file on the SD card
  WIFI_INIT_ERROR = 6,           // Failed to initialize the WiFi. Check the WiFi settings.
  UPLOAD_PICTURE_ERROR = 7,      // Failed to upload the picture. Check the upload settings.
  READ_CONFIG_ERROR = 8          // Failed to read the configuration file on SD card.
                                 // Check the content of your configuration file.
} status_code_t;

/**
 * @brief Switch on the built-in red led.
 */
void switchOnRedLed();

/**
 * @brief Switch off the built-in red led.
 */
void switchOffRedLed();

/**
 * @brief Log the given status code and transmit its
 *        value to the user via the built-in red led.
 *
 * @param statusCode
 *
 * @see status_code_t
 */
void signalError(status_code_t code);

#endif