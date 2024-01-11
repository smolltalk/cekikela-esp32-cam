#ifndef OTA_H
#define OTA_H

#include <Arduino.h>
#include <HTTPUpdate.h>
#include "logging.h"
#include "wifimgt.h"

// Logger name for this module
#define OTA_LOG "OTA"

// Maximum length of the firmware update URL
#define OTA_FIRWARE_UPDATE_URL_SIZE 96
// Default value in hours of the firmware update check period
#define OTA_CHECK_PERIOD_HOURS_DEFAULT 24

/**
 * Over The Air (OTA) firmware update settings.
 */
typedef struct {
  uint8_t checkPeriodHours;          // Firmware update check period in hours. 
  char url[OTA_FIRWARE_UPDATE_URL_SIZE]; // Firmware update URL.
} ota_settings_t;

/**
 * @brief Check if a firmware update is available and install it.
 *
 * @param wifi           WiFi settings required to connect to NTP servers
 * @param ota            OTA settings
 * @param currentVersion current version of the application
 */
void updateFirmware(wifi_settings_t * wifi, ota_settings_t * ota, char * currentVersion);

#endif