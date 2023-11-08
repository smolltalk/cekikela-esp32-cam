#include "wifimgt.h"

statusCode initWifi(wifiSettings_t * wifiSettings) {
  statusCode result = ok;

  // Disabled?
  if (!wifiSettings->enabled){
    result = wifiInitError;
    Serial.printf("Wifi is disabled. Return with code: %d\n", result);
    return result;
  }
  
  // Else, enabled:
  // We start by connecting to a WiFi network
  Serial.println();
  // Already connected? Cool. Return.
  if (WiFi.status() == WL_CONNECTED){
    Serial.println("WiFi already connected.");
    return result;
  }
  // Try to connect
  Serial.print("Connecting to ");
  Serial.println(wifiSettings->ssid);

  WiFi.begin(wifiSettings->ssid, wifiSettings->password);

  int connectAttemptCount = 0;
  while (WiFi.status() != WL_CONNECTED && connectAttemptCount < wifiSettings->connectAttemptMax) {
    delay(500);
    Serial.print(".");
    connectAttemptCount++;
  }

  Serial.println("");

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("WiFi connected.");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
  } else {
    result = wifiInitError;
    Serial.println("Wifi NOT connected.");
  }

  return result;
}

void endWifi() {
  WiFi.disconnect();
}
