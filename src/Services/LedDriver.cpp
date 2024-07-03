#include "LedDriver.h"

#include <FastLED.h>

#define DATA_PIN D4

namespace Services {
  const char *LedDriver::Name = "led";

  LedDriver::LedDriver(uint ledCount) : ledCount(ledCount) {
    leds = new CRGB[ledCount];
  }

  void LedDriver::setBrightness(uint8_t brightness) {
    this->brightness = brightness;
    FastLED.setBrightness(brightness);
  }

  void LedDriver::setup() {
    FastLED.addLeds<WS2812B, DATA_PIN, RGB>(leds, ledCount);
    FastLED.setBrightness(brightness);
    FastLED.clear();
  }

  void LedDriver::loop() {
    fill_solid(leds, ledCount, CRGB::Red);
    FastLED.show();
  }

}