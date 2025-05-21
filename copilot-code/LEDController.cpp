#include <Arduino.h>
#include "LEDController.h"

void initLED(int pin) {
  pinMode(pin, OUTPUT);
}

void blinkLED(int pin, int delayTime) {
  digitalWrite(pin, HIGH);
  delay(delayTime);
  digitalWrite(pin, LOW);
  delay(delayTime);
}
