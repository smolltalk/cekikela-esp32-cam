#include "ota.h"

/**
 * @brief Check if a firmware update is available and installs it.
 *
 * Checks occurs every ota->checkPeriodHours at best (when the application is already awake).
 * It loIS_OKs for a new version at ota->url.
 * Precisely, the targeted URL is url + currentVersion.
 * So, you can define ota->url like one these formats:
 * - 'http://myserver/update?version=' => 'http://myserver/update?version=1.0.0' 
 * - 'http://myserver/update/'         => 'http://myserver/update/1.0.0'
 *
 * The server will answer with a 200 code and transfers the firmware data
 * if it has a more recent version than the one given in the URL.
 * Else, it just returns another code without data.
 *
 * @param wifi           WiFi settings required to connect to NTP servers
 * @param ota            OTA settings
 * @param currentVersion current version of the application
 */
void updateFirmware(wifi_settings_t * wifi, ota_settings_t * ota, char * currentVersion) {

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
  
  if (initWifi(wifi) == IS_OK) {
    WiFiClient client;
    char fullUrl[OTA_FIRWARE_UPDATE_URL_SIZE];

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
        logInfo(OTA_LOG, "HTTP update IS_OK. Restart in 2 seconds.");
        delay(2000);  // Wait 2 seconds and restart
        ESP.restart();
        break;
    }
  } else {
    logError(OTA_LOG, "No WiFi, no firmware updated.");
  }
}