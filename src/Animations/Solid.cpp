#include "Solid.h"

namespace Animations {
  void Solid::getFields(Web::UI::FieldsInfo& fields) {
    fields.addColorChooser("color", "Color");
  }

  void Solid::getSettings(JsonObject& settings) const {
    settings["color"]["number"] = color.number;
    JsonArray rgb = settings["color"]["rgb"].to<JsonArray>();
    rgb.add(color.rgb.r);
    rgb.add(color.rgb.g);
    rgb.add(color.rgb.b);
  }

  void Solid::setSettings(const JsonObject& settings) {
    if (settings["color"]) {
      color.number = settings["color"]["number"];
      if (color.number > 0 && color.number <= context.getColors().size()) {
        color.rgb = context.getColors()[color.number-1];
      } else {
        JsonArray rgb = settings["color"]["rgb"].as<JsonArray>();
        color.rgb.r = rgb[0];
        color.rgb.g = rgb[1];
        color.rgb.b = rgb[2];
      }
    }
  }

  void Solid::renderFrame() {
    Animation::renderFrame();
  }

  void Solid::renderLed(int index) {
    if (color.number > 0 && color.number <= context.getColors().size()) {
      context.rgb(context.getColors()[color.number-1]);
    } else {
      context.rgb(color.rgb);
    }
  }
}