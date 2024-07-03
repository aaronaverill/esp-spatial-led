#include <Arduino.h>

#include "Application.h"
#include "Services/LedDriver.h"
#include "Animations/Colorloop.h"
#include "Animations/Rainbow.h"
#include "Animations/Solid.h"

void addAdminations(LedDriver& ledDriver) {
  ledDriver.addAnimation(new Animations::Colorloop(ledDriver));
  ledDriver.addAnimation(new Animations::Rainbow(ledDriver));
  ledDriver.addAnimation(new Animations::Solid(ledDriver));
}

void setup() {
  Serial.begin(115200);
  Application& app = Application::getInstance();
  LedDriver* ledDriver = new LedDriver(50);
  addAdminations(*ledDriver);
  app.addService(ledDriver);
  app.setup();
}

void loop() {
  Application::getInstance().loop();
}