#include "LedDriver.h"

#include <FastLED.h>
#include "Store/LedSettings.h"

#define DATA_PIN D4

namespace Services {
  LedDriver *LedDriver::instance = nullptr;

  void LedDriver::setLedCount(uint ledCount) {
    updateLedCount = ledCount;
  }

  void LedDriver::setLedCoordinates(const char *coordinates) {
    // TODO: Get rid of the use of strtok which modifies the input string and change the coordinates parameter to const char*
    ledCoordinates = std::vector<Coordinate>();
    byte pos = 0;
    char *err, *p =(char *)coordinates;
    float parts[3];
    while (*p) {
      parts[pos] = strtod(p, &err);
      if (p == err) {
        p++;
      } else {
        if (pos == 2) {
          ledCoordinates.push_back(Coordinate(parts[0], parts[1], parts[2]));
          Serial.println("Add coordinate (" + String(parts[0]) + "," + String(parts[1]) + "," + String(parts[2]) + ")");
          pos = 0;
        } else {
          pos++;
        }

        if ((err != NULL) && (*err == 0)) {
          break;
        } else {
          p = err + 1;
        }
      }
    }
  }

  const Coordinate& LedDriver::getLedCoordinate(uint index) {
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
    Store::LedSettings::read();
    
    controller = new WS2812B<DATA_PIN, RGB>();
    ledCount = updateLedCount;
    leds = new CRGB[ledCount];
    FastLED.addLeds(controller, leds, ledCount, 0);
    FastLED.setBrightness(brightness);
  }

  void LedDriver::loop() {
    if (updateLedCount != ledCount) {
      ledCount = updateLedCount;
      delete leds;
      leds = new CRGB[ledCount];
      controller->setLeds(leds, ledCount);
    }
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