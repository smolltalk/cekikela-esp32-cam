#ifndef TIMEMGT_H
#define TIMEMGT_H

#include "Arduino.h"
#include "error.h"
#include "logging.h"
#include "time.h"
#include "wifimgt.h"

// Logger name for this module
#define TIME_LOG "Time"

// NTP server max size
#define TIME_NTP_SERVER_MAX_SIZE 64
// Default NTP server
#define TIME_NTP_SERVER_DEFAULT "pool.ntp.org"
// Default GMT offset = 0
#define TIME_GMT_OFFSET_SEC_DEFAULT 0
// Default daylight offset = 0
#define TIME_DAYLIGHT_OFFSET_SEC_DEFAULT 3600

// Sync time period = 24H
#define TIME_SYNC_PERIOD_HOURS_DEFAULT 24

/**
 * Time settings.
 * Usefull to define the NTP server
 * and others NTP parameters.
 * Moreover, time synchronization period can be also defined.
 */
typedef struct {
  bool enabled;                              // True to enable the time synchronization feature.
  char ntpServer[TIME_NTP_SERVER_MAX_SIZE];  // NTP server address
  long gmtOffsetSec;                         // GMT offset in seconds (see configTime)
  int daylightOffsetSec;                     // Daylight offset in seconds (see configTime)
  uint8_t syncTimePeriodHours;               // the time synchronization is done periodically each syncTimePeriodHours
} time_settings_t;


/**
 * @brief Update internal clock thanks to NTP.
 *
 * @param wifi         WiFi support is required for NTP
 * @param timeSettings
 */
void syncTime(wifi_settings_t* wifi, time_settings_t* timeSettings);

#endif