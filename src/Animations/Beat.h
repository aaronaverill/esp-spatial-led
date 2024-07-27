#pragma once

#include <Arduino.h>

namespace Animations {
  /**
   * Accumulate a byte value during specified beats per minute from 0..255. Each bpm the value overflows to zero.
   * Call tick() from within an animation renderFrame().
   */
  class Beat {
    public:
      Beat(float bpm = 60) {
        setBpm(bpm);
      }

      /**
       * Call during animation renderFrame()
       */
      void tick() {
        ulong now = millis();
        if (now - lastMillis >= everyMillis) {
          value += stepIncrement;
          lastMillis = now;
        }
      }

      /**
       * Set the speed which value accumulates from 0..255 in beats per minute
       */
      void setBpm(float bpm) {
        stepIncrement = 1;
        float timeBetween = 234.375/bpm;
        // Don't update more than once every 20 milliseconds
        if (timeBetween < 20) {
          stepIncrement = 20/timeBetween;
          timeBetween *= stepIncrement;
        }
        everyMillis = timeBetween;
      }

      /**
       * The value
       */
      byte value = 0;

    private:
      uint32_t everyMillis;
      byte stepIncrement;
      ulong lastMillis = 0;
  };
}