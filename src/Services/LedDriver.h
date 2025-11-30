#pragma once
#include <vector>
#include <Arduino.h>
#include <FastLED.h>
#include <FS.h>

#include "Animations/Animation.h"
#include "ColorChoice.h"
#include "Palette.h"
#include "Services/ILedInfo.h"
#include "Services/IService.h"
#include "Services/SoundInput.h"
#include "Services/Led/INeoPixelBus.h"

namespace Services {
  /**
   * A service that manages LED hardware, LED mapping and the animation library and playback
   */
  class LedDriver: public IService, public ILedDriverAnimationContext, public ILedInfo {
    public:
      /**
       * Create singleton
       */
      static LedDriver& create(SoundInput& soundInput, FS& fs) {
        if (instance == NULL) {
          instance = new LedDriver(soundInput, fs);
        }
        return *instance;
      }

      /**
       * Get chipset
       */
      Led::Chipset getChipset() const { return strip->getChipset(); }
      /**
       * Set chipset
       */
      void setChipset(Led::Chipset chipset) { this->chipset = chipset; }
      /**
       * Get list of chipset options
       */
      std::vector<String> getChipsetOptions() const;
      /**
       * Get the color order
       */
      Led::ColorOrder getColorOrder() const { return strip->getColorOrder(); }
      /**
       * Set color order
       */
      void setColorOrder(Led::ColorOrder colorOrder) { this->colorOrder = colorOrder; }
      /**
       * Get list of color order
       */
      std::vector<String> getColorOrderOptions() const;
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
       * Get the size in bytes of a single led in the buffer
       */
      size_t getLedSize() const { return strip->pixelSize(); }
      /**
       * Get the coordinate of an led by index
       */
      const Algorithm::Point3D& getLedCoordinate(uint index) const;
      /**
       * Get the polar coordinate of an led by index
       */
      const Algorithm::PointPolar& getLedPolarCoordinate(uint index) const;
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
      void setColor(int index, uint8_t r, uint8_t g, uint8_t b);
      /**
       * Get the palettes
       */
      const std::vector<Palette>& getPalettes() const { return palettes; }
      /**
       * Add a system palette
       */
      void addPalette(String name, const std::vector<Palette::GradientStop>& stops);
      /**
       * Set the system palettes
       */
      void setPalettes(std::vector<Palette>& palettes) { this->palettes = palettes; }
      /**
       * Set the system palette
       */
      void setPalette(int index, String name, const std::vector<Palette::GradientStop>& stops);
      /**
       * Delete a system palette
       */
      void deletePalette(int index);

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

      float* getSoundVolumes() {
        return soundInput.getVolumeByFrequency();
      }

    private:
      Led::INeoPixelBus* createController() const;
      void addDefaultColors();
      void addDefaultPalettes();

      LedDriver(SoundInput& soundInput, FS& fs);
      static LedDriver *instance;
      SoundInput& soundInput;
      FS& fs;

      Led::Chipset chipset = Led::Chipset::WS2812;
      Led::ColorOrder colorOrder = Led::ColorOrder::GRB;
      Led::INeoPixelBus *strip;
      uint ledCount = 50;

      std::vector<Algorithm::Point3D> ledCoordinates;
      mutable std::vector<Algorithm::PointPolar> *ledPolarCoordinates = nullptr;
      uint8_t brightness = 128;

      std::vector<CRGB> colors;
      std::vector<Palette> palettes;

      std::vector<Animations::Animation*> animations;

      Animations::Animation* currentAnimation = nullptr;
      uint currentAnimationIndex = 0;
      uint fpsSampleStartMillis = 0;
      uint fpsFrames = 0;
      float fps = -1;

      uint currentLed = 0;
      mutable Algorithm::Point3D autoCoordinate;
      mutable Algorithm::PointPolar autoPolarCoordinate;
  };
}