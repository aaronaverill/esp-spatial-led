#pragma once

#include <vector>
#include <Arduino.h>
#include <FastLED.h>

/**
 * A gradient palette
 */
class Palette {
  public:
    Palette(const char *name): name(name) {}

    class GradientStop {
      byte percent;
      CRGB rgb;
    };
  private:
    const char *name;
    std::vector<GradientStop> stops;
};