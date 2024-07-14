#include "LedsController.h"
#include <ArduinoJson.h>
#include "Application.h"
#include "Services/LedDriver.h"
#include "Animations/Animation.h"
#include "Store/LedLayout.h"
#include "Store/LedSettings.h"
#include "BmpFile.h"

using namespace Services;

namespace Web { namespace Api {
  void LedsController::setAnimationSetting(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, data);
    if (error) {
      request->send(500);
    }

    JsonVariant animationIndex = doc["index"];
    if (!animationIndex.isNull()) {
      LedDriver& leds = LedDriver::getInstance();
      const std::vector<Animations::Animation*>& animations = leds.getAnimations();
      if (animationIndex < animations.size()) {
        animations[animationIndex]->setSettings(doc.as<JsonObject>());
      }
      Store::LedSettings::write();
    }
    request->send(200, "text/plain", "OK");
  }

  void LedsController::setColor(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, data);
    if (error) {
      request->send(500);
    }

    JsonVariant colorIndex = doc["index"];
    JsonVariant rgb = doc["rgb"];
    if (!colorIndex.isNull() && rgb) {
      LedDriver& leds = LedDriver::getInstance();
      if (colorIndex < leds.getColors().size()) {
        leds.setColor(colorIndex, rgb["r"], rgb["g"] ,rgb["b"]);
      }
      Store::LedSettings::write();
    }
    request->send(200, "text/plain", "OK");
  }

  void LedsController::setSettings(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, data);
    if (error) {
      request->send(500);
    }

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

    LedDriver& leds = LedDriver::getInstance();
    JsonVariant brightness = doc["brightness"];
    if (brightness) {
      leds.setBrightness(brightness);
      Store::LedSettings::write();
    }
    request->send(200, "text/plain", "OK");
  }

  void LedsController::getFps(AsyncWebServerRequest *request) {
    LedDriver& leds = LedDriver::getInstance();
    request->send(200, "text/plain", String(leds.getFramesPerSecond()));
  }

  void LedsController::getRgb(AsyncWebServerRequest *request) {
    BmpFile::writeLedData(request, &Services::LedDriver::getInstance());
  }
}}