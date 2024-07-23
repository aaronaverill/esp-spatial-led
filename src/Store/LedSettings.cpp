#include "LedSettings.h"
#include <ArduinoJson.h>

using namespace Services;

namespace Store {
  const char* filename = "/led/settings.json";

  void LedSettings::read(FS& fs, LedDriver& leds) {
    if (!fs.exists(filename)) return;
    File file = fs.open(filename, "r");
    if (!file) return;
    
    JsonDocument doc;
    if(deserializeJson(doc, file)) return;

    //Serial.println("LedSettings::read()");
    //Serial.println(doc.as<String>());

    JsonVariant ledCount = doc["ledCount"];
    if (ledCount) {
      leds.setLedCount(ledCount);
    }

    JsonVariant brightness = doc["play"]["brightness"];
    if (brightness) {
      leds.setBrightness(brightness);
    }
    JsonVariant animationIndex = doc["play"]["index"];
    if (animationIndex) {
      leds.setCurrentAnimationIndex(animationIndex);
    }

    std::vector<CRGB> colors;
    JsonVariant jsonColors = doc["colors"];
    if (jsonColors) {
      for(JsonVariant color:jsonColors.as<JsonArray>()) {
        colors.push_back(CRGB(color["r"], color["g"], color["b"]));
      }
    }
    leds.setColors(colors);

    JsonArray jsonAnimations = doc["animations"].as<JsonArray>();
    if (jsonAnimations) {
      uint index = 0;
      const std::vector<Animations::Animation*>& animations = leds.getAnimations();
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

  void LedSettings::write(FS& fs, LedDriver& leds) {
    const std::vector<Animations::Animation*>& animations = leds.getAnimations();

    JsonDocument doc;
    doc["ledCount"] = leds.getLedCount();
    doc["play"]["brightness"] = leds.getBrightness();
    doc["play"]["index"] = leds.getCurrentAnimationIndex();

    JsonArray jsonColors = doc["colors"].to<JsonArray>();
    for(CRGB rgb:leds.getColors()) {
      JsonObject jsonColor = jsonColors.add<JsonObject>();
      jsonColor["r"] = rgb.red;
      jsonColor["g"] = rgb.green;
      jsonColor["b"] = rgb.blue;
    }

    JsonArray jsonAnimations = doc["animations"].to<JsonArray>();
    for(Animations::Animation* animation:animations) {
      JsonObject jsonSettings = jsonAnimations.add<JsonObject>();
      animation->getSettings(jsonSettings);     
    }

    //Serial.println("LedSettings::write()");
    //Serial.println(doc.as<String>());

    File file = fs.open(filename, "w");
    serializeJson(doc, file);
    file.close();
  }
}