#ifndef OTA_H
#define OTA_H

#include <Arduino.h>
#include <HTTPUpdate.h>
#include "logging.h"
#include "wifimgt.h"

#define OTA_LOG "OTA"
#define FIRWARE_UPDATE_URL_SIZE 96

void updateFirmware(wifiSettings_t * wifi, char * updateUrl, char * currentVersion);

#endif