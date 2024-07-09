#pragma once

namespace Store {
  /**
   * Data store for led settings
   */
  class LedSettings {
    public:
      static void read();
      static void write();
  };
}