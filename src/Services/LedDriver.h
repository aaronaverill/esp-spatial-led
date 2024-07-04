#pragma once
#include <Arduino.h>
#include <FastLED.h>
#include "Service.h"
#include "Animations/Animation.h"

#include <vector>

namespace Services {
  class LedDriver: public Service, public ILedDriverAnimationContext {
    public:
      static const char *Name;

      LedDriver(uint ledCount);

      const uint getLedCount() { return ledCount; }
      CRGB* getLeds() { return leds; }
      void setLedCoordinates(char *coordinates);
      Coordinate& getLedCoordinate(uint index);

      const uint8_t getBrightness() { return brightness; }
      void setBrightness(uint8_t brightness);

      const uint getCurrentLed() { return currentLed; }
      void setCurrentLed(uint index) { currentLed = index; }
      void hsv(const CHSV& hsv);
      void rgb(const CRGB& rgb);

      void addAnimation(Animations::Animation* animation);
      std::vector<Animations::Animation*>& getAnimations() { return animations; }
      Animations::Animation* getCurrentAnimation() { return currentAnimation; }
      const uint getCurrentAnimationIndex() { return currentAnimationIndex; }
      void setCurrentAnimationIndex(uint index);

      void setup();
      void loop();

    private:
      uint ledCount = 0;
      CRGB *leds;
      std::vector<Coordinate> ledCoordinates;
      uint8_t brightness = 128;

      std::vector<Animations::Animation*> animations;
      Animations::Animation* currentAnimation = nullptr;
      uint currentAnimationIndex = 0;

      uint currentLed = 0;
  };
}