#pragma once
#include <Arduino.h>

namespace Services {
  /**
   * Base class for all services
   */
  class Service {
    public:
      Service(const char *name = "") : name(name) {}
      virtual void setup() {}
      virtual void loop() {}
      const char *name;
  };
}