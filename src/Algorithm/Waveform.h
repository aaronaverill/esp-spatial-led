#pragma once

#include <Arduino.h>
#include "Algorithm/Math.h"

#define PI2 PI*2

namespace Algorithm {
  /**
   * Waveform algorithms
   */
  class Waveform {
    public:
      /**
       * Generate a periodic sine wave in the range of [0..1] based on the input value which also ranges from [0..1]. 
       * Useful for pulse effects.
       */
      static float sine(float v) {
        // TODO: Research optimizing this using FastLED sin16() or a lookup table
        return (1+sin(v*PI2))/2;
      }

      /**
       * Generate a periodic sawtooth wave in the range of [0..1] based on the input value which also ranges from [0..1].
       * Return value linearly rises from 0 to 1 then drops back to zero.
       */
      static float sawtooth(float v) {
        if (v < 0) {
          v = 1 + Algorithm::Math::fraction(v);
        } else if (v > 1) {
          v = Algorithm::Math::fraction(v);
        }
        return v;
      }

      /**
       * Generate a periodic triangle wave in the range of [0..1] based on the input value which also ranges from [0..1].
       * Return value linearly rises from 0 to 1 until input is 0.5 then falls linearly back to zero.
       */
      static float triangle(float v) {
        if (v < 0) {
          v = 1 + Algorithm::Math::fraction(v);
        } else if (v > 1) {
          v = Algorithm::Math::fraction(v);
        }
        float x = 1.f - fabsf(v*2 - 1);
        return x;
      }

      /**
       * Alias for sine(). Provided for backwards compatibility.
       */
      static float wave(float v) {
        return sine(v);
      }

  };
}