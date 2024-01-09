#ifndef TIMEMGT_H
#define TIMEMGT_H

#include "Arduino.h"
#include "error.h"
#include "logging.h"
#include "time.h"
#include "wifimgt.h"

#define TIME_LOG "Time"

#define NTP_SERVER "pool.ntp.org"
#define GMT_OFFSET_SEC 0
#define DAYLIGHT_OFFSET_SEC 3600

// Sync time period = 24H
#define SYNC_TIME_PERIOD_HOURS_DEFAULT 24

void updateTime(wifiSettings_t* wifi, uint8_t syncTimePeriodHours);

#endif