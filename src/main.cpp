#include <Arduino.h>
#include "ESPAsyncWebServer.h"

#include "Application.h"
#include "Services/LedDriver.h"
#include "Animations/Colorloop.h"
#include "Animations/Rainbow.h"
#include "Animations/Solid.h"
#include "Services/WebServer.h"
#include "Web/Api/LedsController.h"
#include "Web/UI/HomePage.h"

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

  WebServer* webServer = new WebServer();
  webServer->addRequestHandler("/api/leds/current", HTTP_POST, &Web::Api::LedsController::setCurrent);
  webServer->addRequestHandler("/api/leds", HTTP_GET, &Web::Api::LedsController::get);
  webServer->addRequestHandler("/", HTTP_GET, &Web::UI::HomePage::get);
  app.addService(webServer);

  app.setup();
}

void loop() {
  Application::getInstance().loop();
}