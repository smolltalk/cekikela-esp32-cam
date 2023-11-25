#ifndef APP_H
#define APP_H

#include "cfgmgt.h"
#include "error.h"

#define APP_LOG "App"

#define uS_TO_S_FACTOR 1000000  /* Conversion factor for micro seconds to seconds */

void zzzzZZZZ();
statusCode runActions();
void setupAppConfig(appConfig_t * appConfig);

#endif