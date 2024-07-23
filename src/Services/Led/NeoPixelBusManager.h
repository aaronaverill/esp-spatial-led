#pragma once

#include <NeoPixelBus.h>
#include <Fastled.h>
#include "Services/Led/INeoPixelBus.h"
#include "Services/ILedInfo.h"

namespace Services { namespace Led {      
  template<typename T_COLOR_FEATURE, typename T_METHOD> class NeoPixelBusManager : public INeoPixelBus, public NeoPixelBus<T_COLOR_FEATURE, T_METHOD> {
    public:
      NeoPixelBusManager(Chipset chipset, ColorOrder colorOrder, uint16_t countPixels):
        NeoPixelBus<T_COLOR_FEATURE, T_METHOD>(countPixels), _chipset(chipset), _colorOrder(colorOrder) { }

      ~NeoPixelBusManager() { }

      Chipset getChipset() const {
        return _chipset;
      }

      ColorOrder getColorOrder() const {
        return _colorOrder;
      }

      void begin() {
        NeoPixelBus<T_COLOR_FEATURE, T_METHOD>::Begin();
      }

      void show() {
        NeoPixelBus<T_COLOR_FEATURE, T_METHOD>::Show();
      }

      void clear() {
        NeoPixelBus<T_COLOR_FEATURE, T_METHOD>::ClearTo(0);
      }

      void setPixelColorRGB(Services::ILedInfo *leds, uint16_t index, uint8_t r, uint8_t g, uint8_t b) {
        uint16_t brightness = leds->getBrightness();
        if (brightness == 255) {
          NeoPixelBus<T_COLOR_FEATURE, T_METHOD>::SetPixelColor(index, RgbColor(r, g, b));
        } else {
          uint16_t red = ((uint16_t)r * brightness) / 255;
          uint16_t green = ((uint16_t)g * brightness) / 255;
          uint16_t blue = ((uint16_t)b * brightness) / 255;
          NeoPixelBus<T_COLOR_FEATURE, T_METHOD>::SetPixelColor(index, RgbColor(red, green, blue));
        }
      }

      void setPixelColorHSV(Services::ILedInfo *leds, uint16_t index, uint8_t h, uint8_t s, uint8_t v) {
        CHSV hsv(h, s, v);
        CRGB rgb;
        hsv2rgb_rainbow(hsv, rgb);
        setPixelColorRGB(leds, index, rgb.r, rgb.g, rgb.b);
      }

      uint16_t pixelCount() const {
        return NeoPixelBus<T_COLOR_FEATURE, T_METHOD>::PixelCount();
      }

      const uint8_t * getPixelData() const {
        return NeoPixelBus<T_COLOR_FEATURE, T_METHOD>::_pixels();
      }

    private:
      Chipset _chipset;
      ColorOrder _colorOrder;
  };
}}
