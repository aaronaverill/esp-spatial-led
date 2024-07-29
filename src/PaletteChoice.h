#pragma once

#include <Arduino.h>
#include "ColorChoice.h"

/**
 * Represents a numbered global palette or color choice
 */
class PaletteChoice {
  public:
    void getSetting(JsonObject& json) const {
      json["number"] = number;
      json["colorNumber"] = color.number;
      JsonArray jsonRgb = json["rgb"].to<JsonArray>();
      jsonRgb.add(color.rgb.r);
      jsonRgb.add(color.rgb.g);
      jsonRgb.add(color.rgb.b);
    }

    void setSetting(const JsonVariant& json, const std::vector<CRGB>& colors, const std::vector<Palette>& palettes) {
      number = json["number"];
      color.number = json["colorNumber"];
      if (color.number > 0 && color.number <= colors.size()) {
        color.rgb = colors[color.number-1];
      } else {
        JsonArray jsonRgb = json["rgb"].as<JsonArray>();
        color.rgb.r = jsonRgb[0];
        color.rgb.g = jsonRgb[1];
        color.rgb.b = jsonRgb[2];
      }
    }

    uint16_t number = 1;
    ColorChoice color;
};