#include "wifimgt.h"

statusCode initWifi(wifiInfo_t * wifiInfo) {
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
  Serial.println(wifiInfo->ssid);

  WiFi.begin(wifiInfo->ssid, wifiInfo->password);

  int connectAttemptCount = 0;
  while (WiFi.status() != WL_CONNECTED && connectAttemptCount < wifiInfo->connectAttemptMax) {
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
