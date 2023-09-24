#include "wifimgt.h"

statusCode initWifi() {
  statusCode result = ok;
  // We start by connecting to a WiFi network
  Serial.println();
  // Already connected? Cool. Return.
  if (WiFi.status() == WL_CONNECTED){
    Serial.println("WiFi already connected.");
    return result;
  }
  // Try to connect
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  int connectAttemptCount = 0;
  while (WiFi.status() != WL_CONNECTED && connectAttemptCount < WIFI_CONNECT_MAX_ATTEMPTS) {
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
    result = uploadWifiInitError;
    Serial.println("Wifi NOT connected.");
  }

  return result;
}

void endWifi() {
  WiFi.disconnect();
}
