#include "LedsController.h"
#include <ArduinoJson.h>
#include "Application.h"
#include "Services/LedDriver.h"

using namespace Services;

namespace Web { namespace Api {
  void LedsController::setPlayIndex(AsyncWebServerRequest *request) {
    LedDriver& leds = LedDriver::getInstance();
    uint index = request->getParam("index")->value().toInt();
    leds.setCurrentAnimationIndex(index);
    request->send(200, "text/plain", "OK");
  }

  void LedsController::setPlaySetting(AsyncWebServerRequest *request) {
    LedDriver& leds = LedDriver::getInstance();
    if (request->hasParam("brightness")) {
      uint value = request->getParam("brightness")->value().toInt();
      leds.setBrightness(value);
    }
    request->send(200, "text/plain", "OK");
  }

  float LedsController::getFps(AsyncWebServerRequest *request) {
    LedDriver& leds = LedDriver::getInstance();
    request->send(200, "text/plain", String(leds.getFramesPerSecond()));
  }
}}