#include <Arduino.h>
#include "Application.h"

#include "Services/FileSystem.h"

#include "Services/LedDriver.h"
#include "Animations/Colorloop.h"
#include "Animations/Rainbow.h"
#include "Animations/Solid.h"
#include "Animations/Spatial/CubeFire.h"
#include "Animations/Spatial/RainbowPlane.h"

#include "Services/WebServer.h"
#include "Web/Api/InfoController.h"
#include "Web/Api/LedsController.h"
#include "Web/UI/HomePage.h"

Application Application::instance = Application();

/**
 * The application constructor adds and configures all necessary services.
 * Modify this function as needed.
 */
Application::Application() {
  // Add the file system service first so other services can load setings
  addService(&FileSystem::create());

  LedDriver& ledDriver = LedDriver::create();
  ledDriver.addAnimation(new Animations::Colorloop(ledDriver));
  ledDriver.addAnimation(new Animations::Rainbow(ledDriver));
  ledDriver.addAnimation(new Animations::Solid(ledDriver));
  ledDriver.addAnimation(new Animations::Spatial::CubeFire(ledDriver));
  ledDriver.addAnimation(new Animations::Spatial::RainbowPlane(ledDriver));
  
  addService(&ledDriver);

  WebServer& webServer = WebServer::create();
  webServer.addRequestHandler("/api/info", HTTP_GET, &Web::Api::InfoController::get);
  webServer.addRequestHandler("/api/leds/animations/settings", HTTP_PATCH, [](AsyncWebServerRequest *r){}, &Web::Api::LedsController::setAnimationSetting);
  webServer.addRequestHandler("/api/leds/colors", HTTP_PUT, [](AsyncWebServerRequest *r){}, &Web::Api::LedsController::setColor);
  webServer.addRequestHandler("/api/leds/play/settings", HTTP_PATCH, [](AsyncWebServerRequest *r){}, &Web::Api::LedsController::setPlaySetting);
  webServer.addRequestHandler("/api/leds/play/fps", HTTP_GET, &Web::Api::LedsController::getFps);
  webServer.addRequestHandler("/api/leds/play", HTTP_POST, &Web::Api::LedsController::setPlayIndex);
  webServer.addRequestHandler("/api/leds/settings", HTTP_PATCH, [](AsyncWebServerRequest *r){}, &Web::Api::LedsController::setSettings);
  webServer.addRequestHandler("", HTTP_GET, &Web::UI::HomePage::get);
  
  addService(&webServer);
}

void Application::addService(IService* service) {
  services.push_back(service);
}

void Application::setup() {
  for (IService *service:services) {
    service->setup();
  }
}

void Application::loop() {
  for (IService *service:services) {
    service->loop();
  }
}