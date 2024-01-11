#include "timemgt.h"

/**
 * @brief Update internal clock thanks to NTP.
 *
 * lastUpdateTime is used to keep the last synchronization time.
 * As this static variable is stored in the RTC memory, it resists deep sleep.
 *
 * @param wifi WiFi support is required for NTP
 * @param timeSettings
 */
void syncTime(wifi_settings_t * wifi, time_settings_t * timeSettings) {
  if (!timeSettings->enabled) {
    logInfo(TIME_LOG, "Time synchronization is disabled.");
    return;
  }

  struct tm tm;
  bool syncTimeFromNtp = false;
  static RTC_DATA_ATTR time_t lastUpdateTime;

  if (getLocalTime(&tm)) {
    // / 3600 to compare hours
    if (difftime(mktime(&tm), lastUpdateTime) / 3600 > timeSettings->syncTimePeriodHours) {
      syncTimeFromNtp = true;
      logInfo(TIME_LOG, "It's time to update time.");
    }
  } else {
    // Never initialized with NTP
    logInfo(TIME_LOG, "Initialize time from NTP.");
    syncTimeFromNtp = true;
  }
  if (syncTimeFromNtp) {
    if (initWifi(wifi) == IS_OK) {
      configTime(timeSettings->gmtOffsetSec, timeSettings->daylightOffsetSec, timeSettings->ntpServer);
      if (getLocalTime(&tm)) {
        logInfo(TIME_LOG, "Time updated.");
        lastUpdateTime = mktime(&tm);
      } else {
        logError(TIME_LOG, "Time could not be updated.");
      }
    } else {
      logError(TIME_LOG, "No WiFi, no NTP, no time updated.");
    }
  }
#ifdef LOG_LEVEL >= LOG_LEVEL_INFO
  logInfo(TIME_LOG, "Time: ");
  Serial.println(&tm, "%A, %B %d %Y %H:%M:%S");
#endif
}