#ifndef WIFIMGT_H
#define WIFIMGT_H

#include "Arduino.h"
#include "error.h"
#include "logging.h"
#include <HTTPClient.h>

// Logger name for this module
#define WIFI_LOG "Wifi"

// Maximum length of the WiFi SSID
#define WIFI_SSID_MAX_SIZE 32
// Maximum length of the WiFi password
#define WIFI_PASSWORD_MAX_SIZE 32
// Maximum connection attempts
#define WIFI_CONNECT_ATTEMPT_MAX 30
/**
 * WiFi settings.
 */
typedef struct {
  bool enabled;                     // True to enable the upload feature.
  char ssid[WIFI_SSID_MAX_SIZE];         // WiFi SSID
  char password[WIFI_PASSWORD_MAX_SIZE]; // WiFi password in clear text
  uint8_t connectAttemptMax;        // Maximum of connection attempts
} wifi_settings_t;

/**
 * @brief Initialize the WiFi connection
 * according to the given WiFi settings.
 * 
 * @param wifi WiFi settings
 *
 * @return IS_OK when the WiFi connection is established. WIFI_INIT_ERROR in case of failure
 */
status_code_t initWifi(wifi_settings_t * wifi);

/**
 * Terminate the WiFi connection.
 */
void endWifi();

/**
 * @brief Display the Mac address of the WiFi device
 *        on the serial port in hexadecimal format.
 *        Ex: 60:0D:CA:FE:BA:BE
 */
void displayMacAddressHex();

/**
 * @brief Display the Mac address of the WiFi device
 *        on the serial port in a decimal array format.
 *        Ex: [96, 13, 202, 254, 186, 190]
 */
void displayMacAddressDecArray();

/**
 * @brief Fill the given array with the Mac address bytes,
 *        in the order they appear with displayMacAddressHex().
 *
 * @param result the byte array to fill. Its length must be at least 6 bytes
 *
 * @see displayMacAddressHex()
 * @see displayMacAddressDecArray()
 * @see WiFi.macAddress()
 */
void fillWithMacAddress(byte * result);

#endif