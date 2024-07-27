#include "LedDriver.h"

#include "Services/Led/NeoPixelBusFactory.h"
#include "Store/LedLayout.h"
#include "Store/LedSettings.h"

using namespace Algorithm;

namespace Services {
  LedDriver *LedDriver::instance = nullptr;

  LedDriver::LedDriver(FS& fs): fs(fs) {
    colors.push_back(CRGB(0xFF, 0x20, 0x00)); // Red
    colors.push_back(CRGB(0xFF, 0xFF, 0x00)); // Yellow
    colors.push_back(CRGB(0x00, 0xFF, 0x90)); // Green
    colors.push_back(CRGB(0x90, 0x00, 0xFF)); // Purple
    autoPolarCoordinate.distance = CoordinateMax;
  }

  std::vector<String> LedDriver::getChipsetOptions() const {
    return Led::NeoPixelBusFactory::getChipsetOptions();
  }
  
  std::vector<String> LedDriver::getColorOrderOptions() const {
    return Led::NeoPixelBusFactory::getColorOrderOptions();
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
    ledCoordinates = std::vector<Point3D>();
    CoordinateType pos = 0;
    char *err, *p =(char *)coordinates;
    CoordinateType parts[3];
    while (*p) {
      parts[pos] = strtof(p, &err) * CoordinateMax;
      if (p == err) {
        p++;
      } else {
        if (pos == 2) {
          ledCoordinates.push_back(Point3D(parts[0], parts[1], parts[2]));
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
    // Force polar coordinates to be recalculated the next time they are requested
    delete ledPolarCoordinates;
    ledPolarCoordinates = nullptr;
  }

  void LedDriver::setColor(int index, byte r, byte g, byte b) {
    colors[index].r = r;
    colors[index].g = g;
    colors[index].b = b;
  }

  const Point3D& LedDriver::getLedCoordinate(uint index) const {
    if (index < strip->pixelCount() && index < ledCoordinates.size()) {
      return ledCoordinates[index];
    } else {
      autoCoordinate.x = autoCoordinate.y = autoCoordinate.z = ((float)index+0.5f)/strip->pixelCount() * CoordinateMax;
      return autoCoordinate;
    }
  }

  const PointPolar& LedDriver::getLedPolarCoordinate(uint index) const {
    if (ledPolarCoordinates == nullptr) {
      // Lazy calculate polar coordinates when first requested
      ledPolarCoordinates = new std::vector<PointPolar>();
      for(uint i = 0; i < ledCoordinates.size(); i++) {
        ledPolarCoordinates->push_back(PointPolar(ledCoordinates[i]));
      }
    }
    if (index < strip->pixelCount() && index < ledPolarCoordinates->size()) {
      return (*ledPolarCoordinates)[index];
    } else {
      autoPolarCoordinate.angle = ((float)index+0.5f)/strip->pixelCount() * CoordinateMax;
      return autoPolarCoordinate;
    }
  }

  void LedDriver::setBrightness(uint8_t brightness) {
    this->brightness = brightness;
  }

  void LedDriver::hsv(uint8_t h, uint8_t s, uint8_t v) {
    strip->setPixelColorHSV(this, currentLed, h, s, v);
  }

  void LedDriver::rgb(uint8_t r, uint8_t g, uint8_t b) {
    strip->setPixelColorRGB(this, currentLed, r, g, b);
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
    
    strip = createController();
  }

  void LedDriver::loop() {
    if (ledCount != strip->pixelCount() || 
      chipset != strip->getChipset() ||
      colorOrder != strip->getColorOrder()) {
      if (ledCount < strip->pixelCount()) {
        strip->clear();
        strip->show();
      }
      delete strip;
      strip = createController();
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
      strip->clear();
      strip->show();
    }
  }

  Led::INeoPixelBus* LedDriver::createController() const {
    Led::INeoPixelBus *controller = Led::NeoPixelBusFactory::create(chipset, colorOrder, ledCount);

    if (controller) {
      controller->begin();
      controller->show();
    }
       
    return controller;
  }
}