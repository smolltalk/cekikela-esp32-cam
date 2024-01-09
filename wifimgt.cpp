#include "wifimgt.h"

statusCode initWifi(wifiSettings_t * wifi) {
  statusCode result = ok;

  // Disabled?
  if (!wifi->enabled){
    result = wifiInitError;
    logInfo(WIFI_LOG, "Wifi is disabled. Return with code: %d\n", result);
    return result;
  }
  
  // Else, enabled:
  // We start by connecting to a WiFi network
  Serial.println();
  // Already connected? Cool. Return.
  if (WiFi.status() == WL_CONNECTED){
    logInfo(WIFI_LOG, "WiFi already connected.");
    return result;
  }
  // Try to connect
  logInfo(WIFI_LOG, "Connecting to Wifi %s", wifi->ssid);

  WiFi.begin(wifi->ssid, wifi->password);

  int connectAttemptCount = 0;
  while (WiFi.status() != WL_CONNECTED && connectAttemptCount < wifi->connectAttemptMax) {
    delay(500);
    Serial.print(".");
    connectAttemptCount++;
  }

  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    logInfo(WIFI_LOG, "WiFi connected.");
    logInfo(WIFI_LOG, "IP address: %s", WiFi.localIP().toString().c_str());
  } else {
    result = wifiInitError;
    logError(WIFI_LOG, "Wifi NOT connected.");
  }

  return result;
}

void endWifi() {
  WiFi.disconnect();
}

void displayMacAddressHex(){
  byte mac[6];
  WiFi.macAddress(mac);
  for (int i = 5; i >= 0; i --){
    Serial.print(mac[i], HEX);
    if (i > 0) {
      Serial.print(":");
    }
  }
}

void displayMacAddressDecArray(){
  byte mac[6];
  WiFi.macAddress(mac);
  Serial.print("[");
  for (int i = 5; i >= 0; i --){
    Serial.print(mac[i]);
    if (i > 0) {
      Serial.print(", ");
    }
  }
  Serial.print("]");
}

void fillReverseMacAddress(byte * result) {
  byte mac[6];
  WiFi.macAddress(mac);
  for(int i = 0; i < 6; i++) result[i] = mac[5-i];
}