#include "FieldsInfo.h"

namespace Web { namespace UI {
  void FieldsInfo::addSlider(const char *id, const char *label, float min, float max, const char *valueTemplate, int decimals) {
    JsonObject field = fields.add<JsonObject>();
    field["id"] = id;
    field["type"] = "slider";
    field["label"] = label;
    field["min"] = min;
    field["max"] = max;
    if (valueTemplate != nullptr) {
      field["template"] = valueTemplate;
    }
    if (decimals >= 0) {
      field["decimals"] = decimals;
    }
  }

  void FieldsInfo::addHueChooser(const char *id, const char *label) {
    JsonObject field = fields.add<JsonObject>();
    field["id"] = id;
    field["type"] = "hue";
    field["label"] = label;
  }
}}