#pragma once

#include <Arduino.h>

#define PI2 PI*2

namespace Algorithm {
  /**
   * Useful math function
   */
  class Math {
    public:
      /**
       * Return the fractional part of the specific value.
       */
      static float fraction(float v) {
        float intPart;
        return modff(v, &intPart);
      }
  };
}