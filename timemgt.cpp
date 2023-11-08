#include "timemgt.h"

void updateTime(wifiSettings_t * wifiSettings) {
  const char* ntpServer = "pool.ntp.org";
  const long gmtOffset_sec = 0;
  const int daylightOffset_sec = 3600;
  struct tm timeSettings;
  bool updateTimeFromNtp = false;
  static RTC_DATA_ATTR time_t lastUpdateTime;

  if (getLocalTime(&timeSettings)) {
    if (difftime(mktime(&timeSettings), lastUpdateTime) > UPDATE_TIME_PERIOD_SEC) {
      updateTimeFromNtp = true;
      Serial.println("It's time to update time.");
    }
  } else {
    // Never initialized with NTP
    Serial.println("Initialize time from NTP.");
    updateTimeFromNtp = true;
  }
  if (updateTimeFromNtp) {
    if (initWifi(wifiSettings) == ok) {
      configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
      if (getLocalTime(&timeSettings)) {
        Serial.println("Time updated.");
        lastUpdateTime = mktime(&timeSettings);
      } else {
        Serial.println("Time could not be updated.");
      }
    } else {
      Serial.println("No WiFi, no NTP, no time updated.");
    }
  }
  Serial.println(&timeSettings, "Time: %A, %B %d %Y %H:%M:%S");
}