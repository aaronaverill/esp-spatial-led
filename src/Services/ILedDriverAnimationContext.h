#pragma once
#include "Coordinate.h"

#include <FastLED.h>

namespace Services {
  /**
   * Adapter class for animation rendering context
   */
  class ILedDriverAnimationContext {
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
       * Get the coordinate of an led by index
       */
      virtual const Coordinate& getLedCoordinate(uint index) const = 0;
      /**
       * Within a rendering context, the current led which is being rendered
       */
      virtual uint getCurrentLed() const = 0;
      /**
       * Set the current led which is being rendered
       */
      virtual void setCurrentLed(uint index) = 0;
      /**
       * Set the current led's color by HSV
       */
      virtual void hsv(uint8_t h, uint8_t s, uint8_t v) = 0;
      /**
       * Set the current led's color by RGB
       */
      virtual void rgb(uint8_t r, uint8_t g, uint8_t b) = 0;
      /**
       * Get the list of system colors
       */
      virtual const std::vector<CRGB>& getColors() const = 0;
      /**
       * Show the strip
       */
      virtual void show() = 0;
  };
}