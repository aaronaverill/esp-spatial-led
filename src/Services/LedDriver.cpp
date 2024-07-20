#include "LedDriver.h"

#include <FastLED.h>
#include "Store/LedSettings.h"
#include "Store/LedLayout.h"

#define DATA_PIN D4
#define CHIPSET WS2812B
#define RGB_ORDER GRB

namespace Services {
  LedDriver *LedDriver::instance = nullptr;

  LedDriver::LedDriver(FS& fs): fs(fs) {
    colors.push_back(CRGB(0xFF, 0x20, 0x00)); // Red
    colors.push_back(CRGB(0xFF, 0xFF, 0x00)); // Yellow
    colors.push_back(CRGB(0x00, 0xFF, 0x90)); // Green
    colors.push_back(CRGB(0x90, 0x00, 0xFF)); // Purple
  }

  void LedDriver::setLedCount(uint count) {
    // Set the pending led count which will be updated in the next cycle of the loop()
    // The CRGB buffer is updated in the loop() later rather than here immediatey because
    // requests to change the led count could come in at any time asynchrnously and we need
    // to prevent animations which are in the middle of rendering from buffer overruns if the
    //  led count is reduced. 
    ledCount = count;
  }

  void LedDriver::setLedCoordinates(const char *coordinates) {
    ledCoordinates = std::vector<Coordinate>();
    byte pos = 0;
    char *err, *p =(char *)coordinates;
    CoordinateType parts[3];
    while (*p) {
      parts[pos] = strtof(p, &err) * CoordinateMax;
      if (p == err) {
        p++;
      } else {
        if (pos == 2) {
          ledCoordinates.push_back(Coordinate(parts[0], parts[1], parts[2]));
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

  void LedDriver::setColor(int index, byte r, byte g, byte b) {
    colors[index].r = r;
    colors[index].g = g;
    colors[index].b = b;
  }

  const Coordinate& LedDriver::getLedCoordinate(uint index) const {
    if (index < renderLedCount && index < ledCoordinates.size()) {
      return ledCoordinates[index];
    } else {
      autoCoordinate.x = autoCoordinate.y = autoCoordinate.z = (index+0.5)/renderLedCount;
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
    Store::LedSettings::read(fs, *this);
    setLedCoordinates(Store::LedLayout::readCoordinates(fs).c_str());
    
    controller = new CHIPSET<DATA_PIN, RGB_ORDER>();
    renderLedCount = ledCount;
    leds = new CRGB[renderLedCount];
    FastLED.addLeds(controller, leds, renderLedCount, 0);
    FastLED.setBrightness(brightness);
  }

  void LedDriver::loop() {
    if (ledCount != renderLedCount) {
      if (ledCount < renderLedCount) {
        FastLED.clear();
        FastLED.show();
      }
      renderLedCount = ledCount;
      delete leds;
      leds = new CRGB[renderLedCount];
      controller->setLeds(leds, renderLedCount);
    }
    if (currentAnimation) {
      if (fps == -1) {
        fps = 0;
        fpsSampleStartMillis = millis();
      }
      currentAnimation->renderFrame();
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