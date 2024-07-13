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
       * @param valueTemplate - A string which is processed through eval() in javascript to display the value
       * @param decimals - Round the display value to the decimals to show or -1 to not round
       */
      void addSlider(const char *id, const char *label, float min, float max, const char *valueTemplate = nullptr, int decimals = -1);
      /**
       * Add a hue chooser field. Displays a slider on the UI whose value is a byte [0..255]
       */
      void addHueChooser(const char *id, const char *label);

    private:
      JsonArray& fields;
  };
}}