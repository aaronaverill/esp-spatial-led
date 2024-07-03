#include <Arduino.h>

#include "Application.h"
#include "Services/LedDriver.h"

void setup() {
  Serial.begin(115200);
  Application& app = Application::getInstance();
  app.addService(new LedDriver(50));
  app.setup();
}

void loop() {
  Application::getInstance().loop();
}