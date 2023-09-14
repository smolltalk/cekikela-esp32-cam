#include "error.h"

void switchOnRedLed() {
  digitalWrite(RED_LED_PIN, LOW);
}

void switchOffRedLed() {
  digitalWrite(RED_LED_PIN, HIGH);
}

void signalError(statusCode code) {
  Serial.printf("Signal error: %d.\n", code);

  // Blink the number of code
  switchOffRedLed();
  for (int i = 0; i < code; i++) {
    delay(250);
    switchOnRedLed();
    delay(250);
    switchOffRedLed();
  }
}