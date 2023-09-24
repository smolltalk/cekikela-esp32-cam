#ifndef WIFIMGT_H
#define WIFIMGT_H

#include "Arduino.h" 
#include <HTTPClient.h>
#include "error.h"
#include "config.h"

statusCode initWifi();
void endWifi();

#endif