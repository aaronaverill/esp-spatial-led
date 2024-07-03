#pragma once
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
      virtual const uint getLedCount();
      /**
       * Get the led array
       */
      virtual CRGB* getLeds();
      /**
       * Within a rending context, the current led which is being rendered
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