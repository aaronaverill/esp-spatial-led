#include "LedDriver.h"

#include <FastLED.h>

#define DATA_PIN D4

namespace Services {
  const char *LedDriver::Name = "led";

  LedDriver::LedDriver(uint ledCount) : Service(Name), ledCount(ledCount) {
    leds = new CRGB[ledCount];
  }

  void LedDriver::setBrightness(uint8_t brightness) {
    this->brightness = brightness;
    FastLED.setBrightness(brightness);
  }

  void LedDriver::hsv(const CHSV& hsv) {
    leds[currentLed] = hsv;
  }

  void LedDriver::rgb(const CRGB& rgb) {
    leds[currentLed] = rgb;
  }

  void LedDriver::addAnimation(Animations::Animation* animation) {
    animations.push_back(animation);
    if (!currentAnimation) {
      currentAnimation = animation;
      currentAnimationIndex = animations.size() - 1;
    }
  }

  void LedDriver::setup() {
    FastLED.addLeds<WS2812B, DATA_PIN, RGB>(leds, ledCount);
    FastLED.setBrightness(brightness);
    FastLED.clear();
  }

  void LedDriver::loop() {
    if (currentAnimation) {
      currentAnimation->render();
    } else {
      FastLED.clear();
      FastLED.show();
    }
  }

}