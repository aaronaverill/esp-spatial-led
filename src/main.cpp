#include <Arduino.h>

#include "Application.h"

void setup() {
  Serial.begin(115200);

  Application::getInstance().setup();
}

void loop() {
  Application::getInstance().loop();
}