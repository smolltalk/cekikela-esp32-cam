#ifndef WIFIMGT_H
#define WIFIMGT_H

#include "Arduino.h"
#include "error.h"
#include "logging.h"
#include <HTTPClient.h>

#define WIFI_LOG "Wifi"

#define SSID_MAX_SIZE 32
#define PASSWORD_MAX_SIZE 32

typedef struct {
  bool enabled;
  char ssid[SSID_MAX_SIZE + 1];
  char password[PASSWORD_MAX_SIZE + 1];
  uint8_t connectAttemptMax;
} wifiSettings_t;

statusCode initWifi(wifiSettings_t * wifi);
void endWifi();

void displayMacAddressHex();
void displayMacAddressDecArray();
void fillReverseMacAddress(byte * result);

#endif