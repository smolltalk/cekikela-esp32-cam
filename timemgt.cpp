#include "timemgt.h"

void updateTime() {
  const char* ntpServer = "pool.ntp.org";
  const long gmtOffset_sec = 0;
  const int daylightOffset_sec = 3600;
  struct tm timeInfo;
  bool updateTimeFromNtp = false;
  static RTC_DATA_ATTR time_t lastUpdateTime;

  if (getLocalTime(&timeInfo)) {
    if (difftime(mktime(&timeInfo), lastUpdateTime) > 30) {
      updateTimeFromNtp = true;
      Serial.println("It's time to update time.");
    }
  } else {
    // Never initialized with NTP
    Serial.println("Initialize time from NTP.");
    updateTimeFromNtp = true;
  }
  if (updateTimeFromNtp) {
    if (initWifi() == ok) {
      configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
      if (getLocalTime(&timeInfo)) {
        Serial.println("Time updated.");
        lastUpdateTime = mktime(&timeInfo);
      } else {
        Serial.println("Time could not be updated.");
      }
    } else {
      Serial.println("No WiFi, no NTP, no time updated.");
    }
  }
  Serial.println(&timeInfo, "Time: %A, %B %d %Y %H:%M:%S");
}