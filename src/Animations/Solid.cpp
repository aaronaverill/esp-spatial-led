#include "Solid.h"

namespace Animations {
  void Solid::getFields(Web::UI::FieldsInfo& fields) {
    fields.addColorChooser("hue", "Hue");
  }

  void Solid::getSettings(JsonObject& settings) {
    settings["hue"] = hsv.hue;
  }

  void Solid::setSettings(std::vector<KeyValuePair<String, String>> values) {
    for(KeyValuePair<String, String>& pair:values) {
      if(pair.key == "hue") {
        hsv.hue = pair.value.toInt();
      }
    }
  }

  void Solid::render(int index) {
    context.hsv(hsv);
  }
}