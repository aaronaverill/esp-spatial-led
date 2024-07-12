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
    if (error) {
      request->send(500);
    }
    //String json = doc.as<String>();
    //Serial.println(json);

    bool writeSettings = false;
    JsonVariant ledCount = doc["ledCount"];
    LedDriver& leds = LedDriver::getInstance();
    if (ledCount) {
      leds.setLedCount(ledCount);
      writeSettings = true;
    }
    JsonVariant coordinateValues = doc["ledLayout"]["coords"];
    if (coordinateValues) {
      leds.setLedCoordinates(coordinateValues);
      Store::LedLayout::write(doc["ledLayout"]["config"], doc["ledLayout"]["coords"]);
    }
    if (writeSettings) {
      Store::LedSettings::write();
    }
    request->send(200, "text/plain", "OK");
  }

  void LedsController::setPlayIndex(AsyncWebServerRequest *request) {
    LedDriver& leds = LedDriver::getInstance();
    uint index = request->getParam("index")->value().toInt();
    leds.setCurrentAnimationIndex(index);
    request->send(200, "text/plain", "OK");
  }

  void LedsController::setPlaySetting(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, data);
    if (error) {
      request->send(500);
    }
    //String json = doc.as<String>();
    //Serial.println(json);

    LedDriver& leds = LedDriver::getInstance();
    JsonVariant brightness = doc["brightness"];
    if (brightness) {
      leds.setBrightness(brightness);
      Store::LedSettings::write();
    }
    request->send(200, "text/plain", "OK");
  }

  void LedsController::setAnimationSetting(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, data);
    if (error) {
      request->send(500);
    }
    //String json = doc.as<String>();
    //Serial.println(json);

    JsonVariant animationIndex = doc["index"];
    if (animationIndex) {
      LedDriver& leds = LedDriver::getInstance();
      std::vector<Animations::Animation*> animations = leds.getAnimations();
      if (animationIndex < animations.size()) {
        animations[animationIndex]->setSettings(doc.to<JsonObject>());
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