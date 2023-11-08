#ifndef WIFIMGT_H
#define WIFIMGT_H

#include "Arduino.h" 
#include <HTTPClient.h>
#include "error.h"

#define SSID_MAX_SIZE 32
#define PASSWORD_MAX_SIZE 32

typedef struct {
  bool enabled = false;
  char ssid[SSID_MAX_SIZE + 1];
  char password[PASSWORD_MAX_SIZE + 1];
  uint8_t connectAttemptMax;
} wifiSettings_t;

statusCode initWifi(wifiSettings_t * wifiSettings);
void endWifi();

#endif