#ifndef OTA_H
#define OTA_H

#include <Arduino.h>
#include <HTTPUpdate.h>
#include "logging.h"
#include "wifimgt.h"

#define OTA_LOG "OTA"
#define FIRWARE_UPDATE_URL_SIZE 96
#define CHECK_PERIOD_HOURS_DEFAULT 24

typedef struct {
  uint8_t checkPeriodHours;
  char url[FIRWARE_UPDATE_URL_SIZE];
} otaSettings_t;

void updateFirmware(wifiSettings_t * wifi, otaSettings_t * ota, char * currentVersion);

#endif