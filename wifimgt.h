#ifndef WIFIMGT_H
#define WIFIMGT_H

#include "Arduino.h" 
#include <HTTPClient.h>
#include "error.h"

#define SSID_MAX_SIZE 32
#define PASSWORD_MAX_SIZE 32

typedef struct {
  char ssid[SSID_MAX_SIZE + 1];
  char password[PASSWORD_MAX_SIZE + 1];
  uint8_t connectAttemptMax;
} wifiInfo_t;

statusCode initWifi(wifiInfo_t * wifiInfo);
void endWifi();

#endif