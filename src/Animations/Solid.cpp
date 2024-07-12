#include "Solid.h"

namespace Animations {
  void Solid::getFields(Web::UI::FieldsInfo& fields) {
    fields.addHueChooser("hue", "Hue");
  }

  void Solid::getSettings(JsonObject& settings) {
    settings["hue"] = hsv.hue;
  }

  void Solid::setSettings(std::vector<KeyValuePair<String, String>> values) {
    for(KeyValuePair<String, String>& pair:values) {
      if(pair.key == "hue") {
        setHue = pair.value.toInt();
      }
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