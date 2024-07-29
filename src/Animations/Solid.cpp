#include "Solid.h"

#include <FastLED.h>

namespace Animations {
  void Solid::getFields(Web::UI::FieldsInfo& fields) {
    fields.addColorChooser("color", "Color");
  }

  void Solid::getSettings(JsonObject& settings) const {
    JsonObject jsonColor = settings["color"].to<JsonObject>();
    color.getSetting(jsonColor);
  }

  void Solid::setSettings(const JsonObject& settings) {
    if (settings["color"]) {
      color.setSetting(settings["color"], context.getColors());
    }
  }

  void Solid::renderLed(int index) {
    if (color.number > 0 && color.number <= context.getColors().size()) {
      CRGB rgb = context.getColors()[color.number-1];
      context.rgb(rgb.r, rgb.g, rgb.b);
    } else {
      context.rgb(color.rgb.r, color.rgb.g, color.rgb.b);
    }
  }
}