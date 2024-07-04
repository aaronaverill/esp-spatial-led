#pragma once
#include <FastLED.h>
#include "Coordinate.h"

namespace Services {
  /**
   * Adapter class for animation rendering context
   */
  class ILedDriverAnimationContext {
    public:
      /**
       * Get the total number of leds
       */
      virtual const uint getLedCount();
      /**
       * Get the led array
       */
      virtual CRGB* getLeds();
      /**
       * Get the coordinate of an led by index
       */
      virtual Coordinate& getLedCoordinate(uint index);
      /**
       * Within a rendering context, the current led which is being rendered
       */
      virtual const uint getCurrentLed();
      /**
       * Set the current led which is being rendered
       */
      virtual void setCurrentLed(uint index);
      /**
       * Set the current led's color by HSV
       */
      virtual void hsv(const CHSV& hsv);
      /**
       * Set the current led's color by RGB
       */
      virtual void rgb(const CRGB& rgb);
  };
}