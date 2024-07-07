#include "LedsController.h"
#include <ArduinoJson.h>
#include "Application.h"
#include "Services/LedDriver.h"
#include "Animations/Animation.h"

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

  void LedsController::setAnimationSetting(AsyncWebServerRequest *request) {
    LedDriver& leds = LedDriver::getInstance();
    if (request->hasParam("index")) {
      uint index = request->getParam("index")->value().toInt();
      std::vector<KeyValuePair> values;
      for(uint i = 0; i < request->params(); i++) {
        if (request->getParam(i)->name() != "index") {
          values.push_back(KeyValuePair(request->getParam(i)->name(), request->getParam(i)->value()));
        }
      }
      std::vector<Animations::Animation*> animations = leds.getAnimations();
      if (index < animations.size()) {
        animations[index]->setSettings(values);
      }
    }
    request->send(200, "text/plain", "OK");
  }

  void LedsController::getFps(AsyncWebServerRequest *request) {
    LedDriver& leds = LedDriver::getInstance();
    request->send(200, "text/plain", String(leds.getFramesPerSecond()));
  }
}}