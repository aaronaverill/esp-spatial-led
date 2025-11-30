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
    CRGB& rgb = color.rgb;
    if (color.number > 0 && color.number <= context.getColors().size()) {
      rgb = context.getColors()[color.number-1];
    }
    float* sound = context.getSoundVolumes();
    sound += index + 1;
    float factor = max(0.f ,min(((*sound) - 200) / 10000, 1.f));
    Serial.println(String(*sound) + "," + String(factor));
    context.rgb((float)color.rgb.r * factor, (float)color.rgb.g * factor, (float)color.rgb.b * factor);
  }
}