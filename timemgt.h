#ifndef TIMEMGT_H
#define TIMEMGT_H

#include "Arduino.h"
#include "error.h"
#include "time.h"
#include "wifimgt.h"

// Update time period = 24h
#define UPDATE_TIME_PERIOD_SEC 86400

void updateTime(wifiSettings_t * wifi);

#endif