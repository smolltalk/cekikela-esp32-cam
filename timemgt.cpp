#include "timemgt.h"

void updateTime(wifiSettings_t * wifi) {
  struct tm timeSettings;
  bool updateTimeFromNtp = false;
  static RTC_DATA_ATTR time_t lastUpdateTime;

  if (getLocalTime(&timeSettings)) {
    if (difftime(mktime(&timeSettings), lastUpdateTime) > UPDATE_TIME_PERIOD_SEC) {
      updateTimeFromNtp = true;
      logInfo(TIME_LOG, "It's time to update time.");
    }
  } else {
    // Never initialized with NTP
    logInfo(TIME_LOG, "Initialize time from NTP.");
    updateTimeFromNtp = true;
  }
  if (updateTimeFromNtp) {
    if (initWifi(wifi) == ok) {
      configTime(GMT_OFFSET_SEC, DAYLIGHT_OFFSET_SEC, NTP_SERVER);
      if (getLocalTime(&timeSettings)) {
        logInfo(TIME_LOG, "Time updated.");
        lastUpdateTime = mktime(&timeSettings);
      } else {
        logError(TIME_LOG, "Time could not be updated.");
      }
    } else {
      logError(TIME_LOG, "No WiFi, no NTP, no time updated.");
    }
  }
  logInfo(TIME_LOG, "Time: %A, %B %d %Y %H:%M:%S", &timeSettings);
}