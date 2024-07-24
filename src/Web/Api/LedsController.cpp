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
        leds.setColor(colorIndex, rgb[0], rgb[1] ,rgb[2]);
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
    JsonVariant count = doc["count"];
    if (count) {
      leds.setLedCount(count);
      writeSettings = true;
    }
    JsonVariant coordinates = doc["coordinates"];
    if (coordinates) {
      leds.setLedCoordinates(coordinates);
      Store::LedLayout::write(fs, doc["layout"], doc["coordinates"]);
    }
    if (writeSettings) {
      Store::LedSettings::write(fs, leds);
    }
    request->send(200, "text/plain", "OK");
  }

  void LedsController::setPlayIndex(AsyncWebServerRequest *request) {
    uint index = request->getParam("index")->value().toInt();
    leds.setCurrentAnimationIndex(index);
    Store::LedSettings::write(fs, leds);
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
      if (brightness >= 253) {
        leds.setBrightness(255);
      } else {
        leds.setBrightness(brightness);
      }
      Store::LedSettings::write(fs, leds);
    }
    request->send(200, "text/plain", "OK");
  }

  void LedsController::getRgb(AsyncWebServerRequest *request) const {
    BmpFile::writeLedData(request, &leds);
  }
}}