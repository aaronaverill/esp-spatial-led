#pragma once

#include <Arduino.h>
#include <FastLED.h>

/**
 * Represents a color that is either a numbered global color or an rgb
 */
struct ColorChoice {
  uint16_t number = 0;
  CRGB rgb;
};