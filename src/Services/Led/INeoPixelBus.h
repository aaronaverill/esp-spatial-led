#pragma once
#include <Arduino.h>
#include "Services/ILedInfo.h"

namespace Services { namespace Led {

  enum Chipset { WS2812, WS2813 };
  
  enum ColorOrder { RGB, RBG, GBR, GRB, BGR, BRG };

  /**
   * An interface allowing generic access to the common functions in concrete NeoPixelBus classes.
   */
  class INeoPixelBus {
    public:
      virtual ~INeoPixelBus() {};
      virtual Chipset getChipset() const = 0;
      virtual ColorOrder getColorOrder() const = 0;
      virtual void begin() = 0;
      virtual void show() = 0;
      virtual void clear() = 0;
      virtual void setPixelColorRGB(ILedInfo *leds, uint16_t index, uint8_t r, uint8_t g, uint8_t b) = 0;
      virtual void setPixelColorHSV(ILedInfo *leds, uint16_t index, uint8_t h, uint8_t s, uint8_t v) = 0;
      virtual uint16_t pixelCount() const = 0;
      virtual const uint8_t * getPixelData() const = 0;
  };
}}