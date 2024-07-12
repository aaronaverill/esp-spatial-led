#include "FieldsInfo.h"

namespace Web { namespace UI {
  void FieldsInfo::addSlider(const char *id, const char *label, int min, int max) {
    JsonObject field = fields.add<JsonObject>();
    field["id"] = id;
    field["type"] = "slider";
    field["label"] = label;
    field["min"] = min;
    field["max"] = max;
  }

  void FieldsInfo::addHueChooser(const char *id, const char *label) {
    JsonObject field = fields.add<JsonObject>();
    field["id"] = id;
    field["type"] = "hue";
    field["label"] = label;
  }
}}