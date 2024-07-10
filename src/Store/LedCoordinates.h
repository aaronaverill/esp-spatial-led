#pragma once
#include <Arduino.h>

namespace Store {
  /**
   * Data store for led coordinates
   */
  class LedCoordinates {
    public:
      static String readConfig();
      static String readValues();
      static void write(const char * config, const char * values);
  };
}