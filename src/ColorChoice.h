#pragma once

#include <Arduino.h>
#include <FastLED.h>

/**
 * Represents a color that is either a numbered global color or an rgb
 */
class ColorChoice {
  public:
    ColorChoice(uint16_t number = 0): number(number) {}

    void getSetting(JsonObject& json) const {
      json["color"] = number;
      JsonArray jsonRgb = json["rgb"].to<JsonArray>();
      jsonRgb.add(rgb.r);
      jsonRgb.add(rgb.g);
      jsonRgb.add(rgb.b);
    }

    void setSetting(const JsonVariant& json, const std::vector<CRGB>& colors) {
      number = json["color"];
      if (number > 0 && number <= colors.size()) {
        rgb = colors[number-1];
      } else {
        JsonArray jsonRgb = json["rgb"].as<JsonArray>();
        rgb.r = jsonRgb[0];
        rgb.g = jsonRgb[1];
        rgb.b = jsonRgb[2];
      }
    }

    uint16_t number;
    CRGB rgb;
};