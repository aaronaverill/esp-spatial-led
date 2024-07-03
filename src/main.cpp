#include <Arduino.h>

#include "Application.h"

void setup() {
  Serial.begin(115200);
  Application& app = Application::getInstance();
  app.setup();
}

void loop() {
  Application::getInstance().loop();
}