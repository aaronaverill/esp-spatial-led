#include "LedsController.h"
#include <ArduinoJson.h>
#include "Application.h"
#include "Services/LedDriver.h"
#include "Animations/Animation.h"
#include "Store/LedLayout.h"
#include "Store/LedSettings.h"

using namespace Services;

namespace Web { namespace Api {
  void LedsController::setSettings(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, data);
    if (!error) {
      //String json = doc.as<String>();
      //Serial.println(json);

      LedDriver& leds = LedDriver::getInstance();
      bool writeSettings = false;
      JsonVariant ledCount = doc["ledCount"];
      if (ledCount) {
        leds.setLedCount(ledCount);
        writeSettings = true;
      }
      JsonVariant coordinateValues = doc["coordinates"]["values"];
      if (coordinateValues) {
        leds.setLedCoordinates(coordinateValues);
        Store::LedLayout::write(doc["coordinates"]["config"], doc["coordinates"]["values"]);
      }
      if (writeSettings) {
        Store::LedSettings::write();
      }
      request->send(200, "text/plain", "OK");
    }
  }

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
      Store::LedSettings::write();
    }
    request->send(200, "text/plain", "OK");
  }

  void LedsController::setAnimationSetting(AsyncWebServerRequest *request) {
    LedDriver& leds = LedDriver::getInstance();
    if (request->hasParam("index")) {
      uint index = request->getParam("index")->value().toInt();
      std::vector<KeyValuePair<String, String>> values;
      for(uint i = 0; i < request->params(); i++) {
        if (request->getParam(i)->name() != "index") {
          values.push_back(KeyValuePair<String, String>(request->getParam(i)->name(), request->getParam(i)->value()));
        }
      }
      std::vector<Animations::Animation*> animations = leds.getAnimations();
      if (index < animations.size()) {
        animations[index]->setSettings(values);
      }
      Store::LedSettings::write();
    }
    request->send(200, "text/plain", "OK");
  }

  void LedsController::getFps(AsyncWebServerRequest *request) {
    LedDriver& leds = LedDriver::getInstance();
    request->send(200, "text/plain", String(leds.getFramesPerSecond()));
  }
}}