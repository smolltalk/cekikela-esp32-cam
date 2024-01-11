#include "error.h"

/**
 * @brief Switch on the built-in red led.
 */
void switchOnRedLed() {
  digitalWrite(RED_LED_PIN, LOW);
}

/**
 * @brief Switch off the built-in red led.
 */
void switchOffRedLed() {
  digitalWrite(RED_LED_PIN, HIGH);
}

/**
 * @brief Log the given status code and transmit its
 *        value to the user via the built-in red led.
 *
 * It causes the built-in LED to flash 
 * as many times as the numerical value associated 
 * with the status code.
 * 250ms off
 * 350ms on
 * 0 blinks when statusCode is IS_OK.
 *
 * @param statusCode
 *
 * @see status_code_t
 */
void signalError(status_code_t statusCode) {
  logInfo(ERROR_LOG, "Signal error: %d.", statusCode);

  // Blink the number of code
  switchOffRedLed();
  for (int i = 0; i < statusCode; i++) {
    delay(250);
    switchOnRedLed();
    delay(350);
    switchOffRedLed();
  }
}