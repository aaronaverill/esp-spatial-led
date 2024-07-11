#pragma once
#include <Arduino.h>

namespace Store {
  /**
   * Data store for led coordinates
   */
  class LedLayout {
    public:
      static String readLayout();
      static String readCoordinates();
      static void write(const char *layout, const char *coordinates);
  };
}