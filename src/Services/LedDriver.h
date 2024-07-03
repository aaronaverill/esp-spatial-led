#pragma once
#include <Arduino.h>
#include <FastLED.h>
#include "Service.h"

#include <vector>

namespace Services {
  class LedDriver: public Service {
    public:
      static const char *Name;

      LedDriver(uint ledCount);

      const uint getLedCount() { return ledCount; }
      const uint8_t getBrightness() { return brightness; }
      void setBrightness(uint8_t brightness);

      void setup();
      void loop();

    private:
      uint ledCount = 0;
      CRGB* leds;
      uint8_t brightness = 128;
  };
}