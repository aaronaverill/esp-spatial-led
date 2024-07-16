#include "LedsController.h"
#include <ArduinoJson.h>
#include "Application.h"
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
      const std::vector<Animations::Animation*>& animations = leds.getAnimations();
      if (animationIndex < animations.size()) {
        animations[animationIndex]->setSettings(doc.as<JsonObject>());
      }
      Store::LedSettings::write(fs, leds);
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
      if (colorIndex < leds.getColors().size()) {
        leds.setColor(colorIndex, rgb["r"], rgb["g"] ,rgb["b"]);
      }
      Store::LedSettings::write(fs, leds);
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
    if (ledCount) {
      leds.setLedCount(ledCount);
      writeSettings = true;
    }
    JsonVariant coordinateValues = doc["ledLayout"]["coords"];
    if (coordinateValues) {
      leds.setLedCoordinates(coordinateValues);
      Store::LedLayout::write(fs, doc["ledLayout"]["config"], doc["ledLayout"]["coords"]);
    }
    if (writeSettings) {
      Store::LedSettings::write(fs, leds);
    }
    request->send(200, "text/plain", "OK");
  }

  void LedsController::setPlayIndex(AsyncWebServerRequest *request) {
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

    JsonVariant brightness = doc["brightness"];
    if (brightness) {
      leds.setBrightness(brightness);
      Store::LedSettings::write(fs, leds);
    }
    request->send(200, "text/plain", "OK");
  }

  void LedsController::getFps(AsyncWebServerRequest *request) const {
    request->send(200, "text/plain", String(leds.getFramesPerSecond()));
  }

  void LedsController::getRgb(AsyncWebServerRequest *request) const {
    BmpFile::writeLedData(request, &leds);
  }
}}