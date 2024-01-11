#ifndef APP_H
#define APP_H

#include "cfgmgt.h"
#include "error.h"

// Logger name for this module
#define APP_LOG "App"
// Current application version
#define APP_VERSION "0.1.0"

/**
 * @brief The application starts here.
 */
void setup();

/**
 * @brief Take a picture and save it.
 *
 * @return status_code_t which is used by signalError
 */
status_code_t takeAndSavePicture();

/**
 * @brief Prepare the deep sleep and how to be waked up.
 */
void zzzzZZZZ();

#endif