#pragma once
#include <FS.h>
#include "Services/LedDriver.h"

namespace Store {
  /**
   * Data store for led settings
   */
  class LedSettings {
    public:
      static void read(FS& fs, Services::LedDriver& leds);
      static void write(FS& fs, Services::LedDriver& leds);
  };
}