#include <Arduino.h>
#include "Application.h"

#include "Services/FileSystem.h"

#include "Services/LedDriver.h"
#include "Animations/Colorloop.h"
#include "Animations/Rainbow.h"
#include "Animations/Solid.h"
#include "Animations/Spatial/CubeFire.h"
#include "Animations/Spatial/Rainbowfall.h"

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
  /*
  3D coordinates for a pentagonal icositetrahedron
  [
    [0.515626,0.522226,0.005613],
    [0.825954,0.488325,0.130761],
    [0.809541,0.800067,0.255908],
    [0.499214,0.83397,0.130761],
    [0.179462,0.881035,0.397938],
    [0.035646,0.682419,0.628119],
    [0.002837,0.439408,0.397938],
    [0.146653,0.638022,0.167755],
    [0.251888,0.232318,0.154726],
    [0.214228,0.081458,0.452951],
    [0.540538,0,0.452951],
    [0.578198,0.15086,0.154727],
    [0.891435,0.200897,0.428986],
    [0.788571,0.166326,0.747322],
    [0.894299,0.478266,0.815364],
    [0.997163,0.512838,0.497026],
    [0.819009,0.849614,0.672196],
    [0.565617,1,0.510055],
    [0.356598,0.917675,0.760351],
    [0.609991,0.767287,0.922492],
    [0.264218,0.559233,0.957393],
    [0.10927,0.316023,0.784316],
    [0.378647,0.11808,0.82131],
    [0.533595,0.361289,0.994387],
  ]
  ledDriver.setLedCount(24);
  ledDriver.setLedCoordinates("0.515626,0.522226,0.005613,0.825954,0.488325,0.130761,0.809541,0.800067,0.255908,0.499214,0.83397,0.130761,0.179462,0.881035,0.397938,0.035646,0.682419,0.628119,0.002837,0.439408,0.397938,0.146653,0.638022,0.167755,0.251888,0.232318,0.154726,0.214228,0.081458,0.452951,0.540538,0,0.452951,0.578198,0.15086,0.154727,0.891435,0.200897,0.428986,0.788571,0.166326,0.747322,0.894299,0.478266,0.815364,0.997163,0.512838,0.497026,0.819009,0.849614,0.672196,0.565617,1,0.510055,0.356598,0.917675,0.760351,0.609991,0.767287,0.922492,0.264218,0.559233,0.957393,0.10927,0.316023,0.784316,0.378647,0.11808,0.82131,0.533595,0.361289,0.994387");
  */

  //ledDriver.setLedCount(60);
  //ledDriver.setLedCoordinates("0.927,0.5,0.191,1,0.382,0.382,1,0.618,0.382,1,0.618,0.618,1,0.382,0.618,0.927,0.5,0.809,0.809,0.309,0.882,0.882,0.191,0.691,0.691,0.118,0.809,0.618,0,0.618,0.809,0.073,0.5,0.618,0,0.382,0.691,0.118,0.191,0.882,0.191,0.309,0.809,0.309,0.118,0.618,0.382,0,0.5,0.191,0.073,0.382,0.382,0,0.191,0.309,0.118,0.118,0.191,0.309,0.309,0.118,0.191,0.382,0,0.382,0.382,0,0.618,0.191,0.073,0.5,0.118,0.191,0.691,0.191,0.309,0.882,0.309,0.118,0.809,0.5,0.191,0.927,0.618,0.382,1,0.382,0.382,1,0.382,0.618,1,0.5,0.809,0.927,0.618,0.618,1,0.809,0.691,0.882,0.882,0.809,0.691,0.691,0.882,0.809,0.618,1,0.618,0.618,1,0.382,0.809,0.927,0.5,0.882,0.809,0.309,0.809,0.691,0.118,0.691,0.882,0.191,0.5,0.809,0.073,0.618,0.618,0,0.382,0.618,0,0.191,0.691,0.118,0.118,0.809,0.309,0.309,0.882,0.191,0.382,1,0.382,0.191,0.927,0.5,0.382,1,0.618,0.309,0.882,0.809,0.118,0.809,0.691,0.191,0.691,0.882,0.073,0.5,0.809,0,0.618,0.618,0,0.382,0.618,0,0.382,0.382,0,0.618,0.382,0.073,0.5,0.191");

  // 24 pixel ring
  //ledDriver.setLedCount(24);
  //ledDriver.setLedCoordinates("0.5,1,0,0.629,0.983,0,0.75,0.933,0,0.854,0.854,0,0.933,0.75,0,0.983,0.629,0,1,0.5,0,0.983,0.371,0,0.933,0.25,0,0.854,0.146,0,0.75,0.067,0,0.629,0.017,0,0.5,0,0,0.371,0.017,0,0.25,0.067,0,0.146,0.146,0,0.067,0.25,0,0.017,0.371,0,0,0.5,0,0.017,0.629,0,0.067,0.75,0,0.146,0.854,0,0.25,0.933,0,0.371,0.983,0");

  ledDriver.addAnimation(new Animations::Colorloop(ledDriver));
  ledDriver.addAnimation(new Animations::Rainbow(ledDriver));
  ledDriver.addAnimation(new Animations::Solid(ledDriver));
  ledDriver.addAnimation(new Animations::Spatial::CubeFire(ledDriver));
  ledDriver.addAnimation(new Animations::Spatial::Rainbowfall(ledDriver));
  
  addService(&ledDriver);

  WebServer& webServer = WebServer::create();
  webServer.addRequestHandler("/api/info", HTTP_GET, &Web::Api::InfoController::get);
  webServer.addRequestHandler("/api/leds/animations/settings", HTTP_POST, &Web::Api::LedsController::setAnimationSetting);
  webServer.addRequestHandler("/api/leds/play/settings", HTTP_POST, &Web::Api::LedsController::setPlaySetting);
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