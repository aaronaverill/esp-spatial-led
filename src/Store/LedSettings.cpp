#include "LedSettings.h"
#include <ArduinoJson.h>
#include "Services/FileSystem.h"
#include "Services/LedDriver.h"

using namespace Services;

namespace Store {
  const char* filename = "/led/settings.json";

  void LedSettings::read() {
    FileSystem& fs = FileSystem::getInstance();

    if (!fs.exists(filename)) return;
    File file = fs.open(filename, "r");
    if (!file) return;
    
    JsonDocument doc;
    if(deserializeJson(doc, file)) return;

    //String json = doc.as<String>();
    //Serial.println(json);

    LedDriver& leds = LedDriver::getInstance();

    JsonVariant ledCount = doc["ledCount"];
    if (ledCount) {
      leds.setLedCount(ledCount);
    }
    JsonVariant brightness = doc["play"]["brightness"];
    if (brightness) {
      leds.setBrightness(brightness);
    }
    JsonArray jsonAnimations = doc["animations"].as<JsonArray>();
    if (jsonAnimations) {
      uint index = 0;
      std::vector<Animations::Animation*> animations = leds.getAnimations();
      for(JsonObject settings:jsonAnimations) {
        if (index >= animations.size()) {
          break;
        }
        animations[index]->setSettings(settings);
        index++;
      }
    }

    file.close();
  }

  void LedSettings::write() {
    LedDriver& leds = LedDriver::getInstance();
    std::vector<Animations::Animation*> animations = leds.getAnimations();

    JsonDocument doc;
    doc["ledCount"] = leds.getLedCount();
    doc["play"]["brightness"] = leds.getBrightness();

    JsonArray jsonAnimations = doc["animations"].to<JsonArray>();
    for(Animations::Animation* animation:animations) {
      JsonObject jsonSettings = jsonAnimations.add<JsonObject>();
      animation->getSettings(jsonSettings);     
    }

    FileSystem& fs = FileSystem::getInstance();
    File file = fs.open(filename, "w");
    serializeJson(doc, file);
    file.close();
  }
}