#include "FieldsInfo.h"

namespace Web { namespace UI {
  void FieldsInfo::addSlider(const char *id, const char *label, int min, int max, const char *valueTemplate) {
    JsonObject field = fields.add<JsonObject>();
    field["id"] = id;
    field["type"] = "slider";
    field["label"] = label;
    field["min"] = min;
    field["max"] = max;
    if (valueTemplate != nullptr) {
      field["template"] = valueTemplate;
    }
  }

  void FieldsInfo::addHueChooser(const char *id, const char *label) {
    JsonObject field = fields.add<JsonObject>();
    field["id"] = id;
    field["type"] = "hue";
    field["label"] = label;
  }
}}