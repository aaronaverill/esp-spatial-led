#pragma once
#include <FS.h>

namespace Store {
  /**
   * Data store for led settings
   */
  class LedSettings {
    public:
      static void read(FS& fs);
      static void write(FS& fs);
  };
}