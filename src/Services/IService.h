#pragma once
#include <Arduino.h>

namespace Services {
  /**
   * Interface class for all services
   */
  class IService {
    public:
      virtual void setup() = 0;
      virtual void loop() = 0;
  };
}