#pragma once
#include <Arduino.h>
#include <FS.h>

namespace Store {
  /**
   * Data store for led coordinates
   */
  class LedLayout {
    public:
      static String readLayout(FS& fs);
      static String readCoordinates(FS& fs);
      static void write(FS& fs, const char *layout, const char *coordinates);
  };
}