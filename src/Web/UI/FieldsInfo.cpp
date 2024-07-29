#include "FieldsInfo.h"

namespace Web { namespace UI {
  void FieldsInfo::addSlider(const String &id, const String &label, float min, float max, const char *valueTemplate, int decimals) {
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
    addTypeChooser(id, "hue", label);
  }

  void FieldsInfo::addColorChooser(const char *id, const char *label) {
    addTypeChooser(id, "color", label);
  }

  void FieldsInfo::addPaletteChooser(const char *id, const char *label) {
    addTypeChooser(id, "palette", label);
  }

  void FieldsInfo::addTypeChooser(const char *id, const char *type, const char *label) {
    JsonObject field = fields.add<JsonObject>();
    field["id"] = id;
    field["type"] = type;
    field["label"] = label;
  }
}}