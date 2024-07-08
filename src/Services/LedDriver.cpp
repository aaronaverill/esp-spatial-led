#include "LedDriver.h"

#include <FastLED.h>
#include "Store/LedSettings.h"

#define DATA_PIN D4

namespace Services {
  LedDriver *LedDriver::instance = nullptr;

  void LedDriver::configure(uint ledCount) {
    this->ledCount = ledCount;
  }

  void LedDriver::setLedCoordinates(char *coordinates) {
    ledCoordinates = std::vector<Coordinate>();
    byte pos = 0;
    char *parts[3];
    char *ptr = strtok(coordinates, ",");  // delimiter
    while (ptr != NULL) {
      parts[pos] = ptr;
      if (pos == 2) {
        ledCoordinates.push_back(Coordinate(atof(parts[0]), atof(parts[1]), atof(parts[2])));
        pos = 0;
      } else {
        pos++;
      }
      ptr = strtok(NULL, ",");
    }
  }

  Coordinate& LedDriver::getLedCoordinate(uint index) {
    if (index < ledCount && index < ledCoordinates.size()) {
      return ledCoordinates[index];
    } else {
      autoCoordinate.x = autoCoordinate.y = autoCoordinate.z = (index+0.5)/ledCount;
      return autoCoordinate;
    }
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

  void LedDriver::setCurrentAnimationIndex(uint index) {
    if (index < animations.size()) {
      currentAnimationIndex = index;
      currentAnimation = animations[index];
    }
  }

  void LedDriver::setup() {
    leds = new CRGB[ledCount];
    FastLED.addLeds<WS2812B, DATA_PIN, RGB>(leds, ledCount);
    FastLED.setBrightness(brightness);
    FastLED.clear();

    Store::LedSettings::read();
  }

  void LedDriver::loop() {
    if (currentAnimation) {
      if (fps == -1) {
        fps = 0;
        fpsSampleStartMillis = millis();
      }
      currentAnimation->render();
      fpsFrames++;
      uint now = millis();
      if (now > fpsSampleStartMillis && (now - fpsSampleStartMillis) > 2000) {
        fps = (float)(1000 * fpsFrames) / (now - fpsSampleStartMillis);
        fpsSampleStartMillis = now;
        fpsFrames = 0;
      }
    } else {
      FastLED.clear();
      FastLED.show();
    }
  }

}