#pragma once

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
      virtual const uint8_t *getLeds() const = 0;

      /**
       * Get the current brightness setting
       */
      virtual uint8_t getBrightness() const = 0;
  };
}