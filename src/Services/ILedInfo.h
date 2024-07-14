#pragma once
#include <FastLED.h>

namespace Services {
  /**
   * Interface class to return information about LEDs
   */
  class ILedInfo {
    public:
      /**
       * Get the total number of leds
       */
      virtual uint getRenderLedCount() const = 0;
      /**
       * Get the led array
       */
      virtual CRGB* getLeds() const = 0;
  };
}