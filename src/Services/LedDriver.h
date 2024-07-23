#pragma once
#include <Arduino.h>
#include <FS.h>
#include <FastLED.h>
#include "IService.h"
#include "ILedInfo.h"
#include "Services/Led/INeoPixelBus.h"
#include "Animations/Animation.h"
#include "Color.h"

#include <vector>

namespace Services {
  /**
   * A service that manages LED hardware, LED mapping and the animation library and playback
   */
  class LedDriver: public IService, public ILedDriverAnimationContext, public ILedInfo {
    public:
      /**
       * Create singleton
       */
      static LedDriver& create(FS& fs) {
        if (instance == NULL) {
          instance = new LedDriver(fs);
        }
        return *instance;
      }

      /**
       * Get the total number of leds being rendered
       */
      uint getRenderLedCount() const { return strip->pixelCount(); }
      /**
       * Set the number of leds
       */
      void setLedCount(uint count);
      /**
       * Get the total number of leds
       * This can differ from renderLedCount if loop() hasn't been called yet to update the strip.
       */
      uint getLedCount() const { return ledCount; }
      /**
       * Get the led array
       */
      const uint8_t *getLeds() const { return strip->getPixelData(); }
      /**
       * Get the coordinate of an led by index
       */
      const Coordinate& getLedCoordinate(uint index) const;
      /**
       * Within a rendering context, the current led which is being rendered
       */
      uint getCurrentLed() const { return currentLed; }
      /**
       * Set the current led which is being rendered
       */
      void setCurrentLed(uint index) { currentLed = index; }
      /**
       * Set the current led's color by HSV
       */
      void hsv(uint8_t h, uint8_t s, uint8_t v);
      /**
       * Set the current led's color by RGB
       */
      void rgb(uint8_t r, uint8_t g, uint8_t b);
      
      /**
       * Set the led coordinate map
       * @param coordinates A comma separated list of x,y,z tuples for each led
       */
      void setLedCoordinates(const char *coordinates);
      /**
       * Get the overall led strip brightness
       */
      uint8_t getBrightness() const { return brightness; }
      /**
       * Set the overall led strip brightness
       */
      void setBrightness(uint8_t brightness);

      /**
       * Get the list of system colors
       */
      const std::vector<CRGB>& getColors() const { return colors; }
      /**
       * Set the system colors
       */
      void setColors(std::vector<CRGB>& colors) { this->colors = colors; }
      /**
       * Set the system color
       */
      void setColor(int index, byte r, byte g, byte b);

      /**
       * Add an animation to the sequencer
       */
      void addAnimation(Animations::Animation* animation);
      /**
       * Get a list of all the animations
       */
      const std::vector<Animations::Animation*>& getAnimations() const { return animations; }
      /**
       * Return the currently playing animation
       */
      Animations::Animation* getCurrentAnimation() const { return currentAnimation; }
      /**
       * Get the index of the currently playing animation
       */
      uint getCurrentAnimationIndex() const { return currentAnimationIndex; }
      /**
       * Set the index of the currently playing animation
       */
      void setCurrentAnimationIndex(uint index);
      /**
       * Get the current frames per second playback speed
       */
      float getFramesPerSecond() const { return fps; }

      void show() { strip->show(); }
      /**
       * Methods for the arduino processing loop
       */
      void setup();
      void loop();


    private:
      Led::INeoPixelBus* createController() const;

      LedDriver(FS& fs);
      static LedDriver *instance;
      FS& fs;

      Led::Chipset chipset = Led::Chipset::WS2813;
      Led::ColorOrder colorOrder = Led::ColorOrder::GRB;
      Led::INeoPixelBus *strip;
      uint ledCount = 1;

      std::vector<Coordinate> ledCoordinates;
      uint8_t brightness = 128;

      std::vector<CRGB> colors;

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