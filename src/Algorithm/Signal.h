#pragma once

#include <Arduino.h>

#define PI2 PI*2

namespace Algorithm {
  /**
   * Algorithms
   */
  class Signal {
    public:
      static float time(float interval) {
        float intPart;
        return modff(millis()/(65535*interval), &intPart);
      }

      static float wave(float v) {
        return (1+sin(v*PI2))/2;
      }
  };
}