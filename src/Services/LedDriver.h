#pragma once
#include <Arduino.h>
#include <FastLED.h>
#include "IService.h"
#include "Animations/Animation.h"

#include <vector>

namespace Services {
  /**
   * A service that manages LED hardware, LED mapping and the animation library and playback
   */
  class LedDriver: public IService, public ILedDriverAnimationContext {
    public:
      /**
       * Create singleton
       */
      static LedDriver& create() {
        if (instance == NULL) {
          instance = new LedDriver();
        }
        return *instance;
      }
      /**
       * Singleton accessor
       */
      static LedDriver& getInstance() { return *instance; }

      /**
       * Get the total number of leds being rendered
       */
      const uint getRenderLedCount() { return renderLedCount; }
      /**
       * Set the number of leds
       */
      void setLedCount(uint count);
      /**
       * Get the total number of leds
       * This can differ from renderLedCount if loop() hasn't been called yet to update the strip.
       */
      const uint getLedCount() { return ledCount; }
      /**
       * Get the led array
       */
      const CRGB* getLeds() { return leds; }
      /**
       * Get the coordinate of an led by index
       */
      const Coordinate& getLedCoordinate(uint index);
      /**
       * Within a rendering context, the current led which is being rendered
       */
      const uint getCurrentLed() { return currentLed; }
      /**
       * Set the current led which is being rendered
       */
      void setCurrentLed(uint index) { currentLed = index; }
      /**
       * Set the current led's color by HSV
       */
      void hsv(const CHSV& hsv);
      /**
       * Set the current led's color by RGB
       */
      void rgb(const CRGB& rgb);
      
      /**
       * Set the led coordinate map
       * @param coordinates A comma separated list of x,y,z tuples for each led
       */
      void setLedCoordinates(const char *coordinates);
      /**
       * Get the overall led strip brightness
       */
      const uint8_t getBrightness() { return brightness; }
      /**
       * Set the overall led strip brightness
       */
      void setBrightness(uint8_t brightness);

      /**
       * Add an animation to the sequencer
       */
      void addAnimation(Animations::Animation* animation);
      /**
       * Get a list of all the animations
       */
      const std::vector<Animations::Animation*>& getAnimations() { return animations; }
      /**
       * Return the currently playing animation
       */
      const Animations::Animation* getCurrentAnimation() { return currentAnimation; }
      /**
       * Get the index of the currently playing animation
       */
      const uint getCurrentAnimationIndex() { return currentAnimationIndex; }
      /**
       * Set the index of the currently playing animation
       */
      void setCurrentAnimationIndex(uint index);
      /**
       * Get the current frames per second playback speed
       */
      const float getFramesPerSecond() { return fps; }

      /**
       * Methods for the arduino processing loop
       */
      void setup();
      void loop();

    private:
      LedDriver() {}
      static LedDriver *instance;

      CLEDController* controller;
      uint renderLedCount;
      uint ledCount = 1;
      CRGB *leds = nullptr;

      std::vector<Coordinate> ledCoordinates;
      uint8_t brightness = 128;

      std::vector<Animations::Animation*> animations;

      Animations::Animation* currentAnimation = nullptr;
      uint currentAnimationIndex = 0;
      uint fpsSampleStartMillis = 0;
      uint fpsFrames = 0;
      float fps = -1;

      uint currentLed = 0;
      mutable Coordinate autoCoordinate;
  };
}