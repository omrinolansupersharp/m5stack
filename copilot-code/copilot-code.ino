#include "LEDController.h"

void setup() {
  initLED(13);  // Initialize pin 13 as output
}

void loop() {
  blinkLED(13, 500);  // Blink LED on pin 13 every 500ms
}
