#pragma once

#include <ArduinoJson.h>

namespace Web { namespace UI {
  /**
   * Abstraction of field information that is gathered from the animations to display in the UI
   */
  class FieldsInfo {
    public:
      FieldsInfo(JsonArray& fields) : fields(fields) {}
      /**
       * Add a slider field. Displays a slider on the UI whose value is between min, max
       */
      void addSlider(const char *id, const char *label, int min, int max);
      /**
       * Add a hue chooser field. Displays a slider on the UI whose value is a byte [0..255]
       */
      void addHueChooser(const char *id, const char *label);

    private:
      JsonArray& fields;
  };
}}