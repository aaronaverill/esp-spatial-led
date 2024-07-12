#include "Solid.h"

namespace Animations {
  void Solid::getFields(Web::UI::FieldsInfo& fields) {
    fields.addHueChooser("hue", "Hue");
  }

  void Solid::getSettings(JsonObject& settings) {
    settings["hue"] = hsv.hue;
  }

  void Solid::setSettings(const JsonObject& settings) {
    if (settings["hue"]) {
      setHue = settings["hue"];
    }
  }

  void Solid::renderFrame() {
    hsv.hue = setHue;
    Animation::renderFrame();
  }

  void Solid::renderLed(int index) {
    context.hsv(hsv);
  }
}