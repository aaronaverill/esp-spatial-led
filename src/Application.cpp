#include "Application.h"

#include <Arduino.h>
#include <LittleFS.h>

#include "Services/LedDriver.h"
#include "Animations/Colorloop.h"
#include "Animations/Rainbow.h"
#include "Animations/Solid.h"
#include "Animations/Spatial/CubeFire.h"
#include "Animations/Spatial/Pulse.h"
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
  // Mount the file system which stores configuration files
  FS& fs = LittleFS;
  bool mounted = LittleFS.begin();
  FSInfo fs_info;
  if (mounted && LittleFS.info(fs_info)) {
    Serial.println("LittleFS mounted:");
    Serial.println("  totalBytes=" + String(fs_info.totalBytes));
    Serial.println("  usedBytes=" + String(fs_info.usedBytes));
    Serial.println("  blockSize=" + String(fs_info.blockSize));
    Serial.println("  pageSize=" + String(fs_info.pageSize));
    Serial.println("  maxOpenFiles=" + String(fs_info.maxOpenFiles));
    Serial.println("  maxPathLength=" + String(fs_info.maxPathLength));
  } else {
    Serial.println("LittleFS not mounted.");
  }

  // Create the LedDriver service and add animations
  LedDriver& ledDriver = LedDriver::create(fs);
  ledDriver.addAnimation(new Animations::Colorloop(ledDriver));
  ledDriver.addAnimation(new Animations::Rainbow(ledDriver));
  ledDriver.addAnimation(new Animations::Solid(ledDriver));
  ledDriver.addAnimation(new Animations::Spatial::CubeFire(ledDriver));
  ledDriver.addAnimation(new Animations::Spatial::Pulse(ledDriver));
  ledDriver.addAnimation(new Animations::Spatial::RainbowPlane(ledDriver));
  
  addService(&ledDriver);

  // Create the WebServer service and add request handlers
  WebServer& webServer = WebServer::create();

  // Add HTTP request handlers for the info API
  Web::Api::InfoController infoController(fs, ledDriver);
  webServer.addRequestHandler("/api/info", HTTP_GET, 
    [infoController](AsyncWebServerRequest *request) mutable {
    infoController.get(request);
  });

  // Add HTTP request handlers for the leds API
  Web::Api::LedsController ledsController(fs, ledDriver);
  webServer.addRequestHandler("/api/leds/animations/settings", HTTP_PATCH, 
    [ledsController](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) mutable {
    ledsController.setAnimationSetting(request, data, len, index, total);
  });
  webServer.addRequestHandler("/api/leds/colors", HTTP_PUT,
    [ledsController](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) mutable {
    ledsController.setColor(request, data, len, index, total);
  });
  webServer.addRequestHandler("/api/leds/play/settings", HTTP_PATCH, 
    [ledsController](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) mutable {
    ledsController.setPlaySetting(request, data, len, index, total);
  });
  webServer.addRequestHandler("/api/leds/play/fps", HTTP_GET, 
    [ledsController](AsyncWebServerRequest *request) {
    ledsController.getFps(request);
  });
  webServer.addRequestHandler("/api/leds/play/rgb", HTTP_GET, 
    [ledsController](AsyncWebServerRequest *request) {
    ledsController.getRgb(request);
  });
  webServer.addRequestHandler("/api/leds/play", HTTP_POST, 
    [ledsController](AsyncWebServerRequest *request) mutable {
    ledsController.setPlayIndex(request);
  });
  webServer.addRequestHandler("/api/leds/settings", HTTP_PATCH, 
    [ledsController](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) mutable {
    ledsController.setSettings(request, data, len, index, total);
  });

  // Add HTTP handlers to serve web assets (HTML, CSS, JS)
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