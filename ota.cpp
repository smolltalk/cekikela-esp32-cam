#include "ota.h"

void updateFirmware(wifiSettings_t* wifi, char* updateUrl, char* currentVersion) {

  if (initWifi(wifi) == ok) {
    WiFiClient client;
    char fullUrl[FIRWARE_UPDATE_URL_SIZE];

    sprintf(fullUrl, "%s%s", updateUrl, currentVersion);
    
    switch (httpUpdate.update(client, fullUrl)) {
      case HTTP_UPDATE_FAILED:
        logError(OTA_LOG, "HTTP update failed with the error (%d): %s\n", httpUpdate.getLastError(), httpUpdate.getLastErrorString().c_str());
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