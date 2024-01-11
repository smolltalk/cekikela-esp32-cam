#include "wifimgt.h"

/**
 * @brief Initialize the WiFi connection
 *        according to the given WiFi settings.
 *
 * This function is reentrant, so feel free
 * to call it regardless the connection status.
 * 
 * @param wifi WiFi settings
 *
 * @return IS_OK when the WiFi connection is established. WIFI_INIT_ERROR in case of failure
 */
status_code_t initWifi(wifi_settings_t * wifi) {
  status_code_t result = IS_OK;

  // Disabled?
  if (!wifi->enabled){
    result = WIFI_INIT_ERROR;
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
    result = WIFI_INIT_ERROR;
    logError(WIFI_LOG, "Wifi NOT connected.");
  }

  return result;
}

/**
 * @brief Terminate the WiFi connection.
 */
void endWifi() {
  WiFi.disconnect();
}

/**
 * @brief Display the Mac address of the WiFi device
 *        on the serial port in hexadecimal format.
 *        Ex: 60:0D:CA:FE:BA:BE
 */
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

/**
 * @brief Display the Mac address of the WiFi device
 *        on the serial port in a decimal array format.
 *        Ex: [96, 13, 202, 254, 186, 190]
 */
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

/**
 * @brief Fill the given array with the Mac address bytes,
 *        in the order they appear with displayMacAddressHex()
 *        or displayMacAddressDecArray().
 *
 * result[0] = first mac address byte.
 * This is the equivalent of the byte array returned 
 * by WiFi.macAddress() but reversed.
 *
 * @param result the byte array to fill. Its length must be at least 6 bytes
 *
 * @see displayMacAddressHex()
 * @see displayMacAddressDecArray()
 * @see WiFi.macAddress()
 */
void fillWithMacAddress(byte * result) {
  byte mac[6];
  WiFi.macAddress(mac);
  for(int i = 0; i < 6; i++) result[i] = mac[5-i];
}