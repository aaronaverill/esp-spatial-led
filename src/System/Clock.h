#pragma once

//#include <Arduino.h>

namespace System {
  /**
   * Functions to generate events based on the system clock
   */
  class Clock {
    public:
      /**
       * Return a value that cycles from 0 to 1 over a time span of 65.535 seconds * interval.
       * A smaller interval number will cycle more quickly than a larger one.
       * @param interval - The speed of the cycle. 
       * - To cycle once per second pass in an interval of 1/65.535 = ~0.01526
       * - To cycle once per minute pass in an interval of 60/65.535 = ~0.91554
       * @remarks
       * This function uses floating point division which can be slow. Consider using one of the other integer options.
       */
      static float time(float interval) {
        float intPart;
        return modff(millis()/(65535*interval), &intPart);
      }

      /**
       * Return a value that cycles from 0 to 65,535 over a time span of 65.535 seconds * 16 / interval.
       * A larger interval number will cycle more quickly than a smaller one.
       * @param interval - The speed of the cycle. 
       * - To cycle once per second pass in an interval of 65.535 * 16 = ~1049
       * - To cycle once per minute pass in an interval of 65.535 * 16 / 60 = ~17
       * - An interval of 1 will cycle every 17m 29s
       */
      static uint16_t time16(uint16_t interval) {
        return (((millis() & 0x000FFFFF) * interval) >> 4) & 0xFFFF;
      }
  };
}