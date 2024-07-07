#include "Solid.h"

namespace Animations {
  void Solid::getFields(JsonArray& fields) {
    JsonObject field = fields.add().to<JsonObject>();
    field["id"] = "hue";
    field["type"] = "hue-slider";
    field["label"] = "Hue";
    field["min"] = 0;
    field["max"] = 255;
  }

  void Solid::getSettings(JsonObject& settings) {
    settings["hue"] = hsv.hue;
  }

  void Solid::setSettings(std::vector<KeyValuePair> values) {
    for(KeyValuePair& pair:values) {
      if(pair.key == "hue") {
        hsv.hue = pair.value.toInt();
      }
    }
  }

  void Solid::render(int index) {
    context.hsv(hsv);
  }
}