#include "ota.h"

void updateFirmware(wifiSettings_t * wifi, otaSettings_t * ota, char * currentVersion) {

  static RTC_DATA_ATTR uint32_t lastCheckTimeMs;
  uint32_t currentTimeMs = millis();

  // / 1000 and / 3600 to compare hours
  if (((currentTimeMs - lastCheckTimeMs) / 1000) / 3600 < ota->checkPeriodHours) {
    // It's not yet the time to check for the firmware update.
    return;
  }

  // Else
  lastCheckTimeMs = currentTimeMs;
  logInfo(OTA_LOG, "It's time to check for the firmware update!");
  
  if (initWifi(wifi) == ok) {
    WiFiClient client;
    char fullUrl[FIRWARE_UPDATE_URL_SIZE];

    sprintf(fullUrl, "%s%s", ota->url, currentVersion);
    
    logInfo(OTA_LOG, "Check for the firmware update at %s...", fullUrl);

    switch (httpUpdate.update(client, fullUrl)) {
      case HTTP_UPDATE_FAILED:
        logError(OTA_LOG, "HTTP update failed with the error (%d): %s", httpUpdate.getLastError(), httpUpdate.getLastErrorString().c_str());
        break;

      case HTTP_UPDATE_NO_UPDATES:
        logInfo(OTA_LOG, "HTTP_UPDATE_NO_UPDATES");
        break;

      case HTTP_UPDATE_OK:
        logInfo(OTA_LOG, "HTTP update OK. Restart in 2 seconds.");
        delay(2000);  // Wait 2 seconds and restart
        ESP.restart();
        break;
    }
  } else {
    logError(OTA_LOG, "No WiFi, no firmware updated.");
  }
}