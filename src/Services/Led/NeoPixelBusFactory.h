#pragma once

#include "Services/Led/INeoPixelBus.h"

namespace Services { namespace Led {
  /**
   * Factory class that instantiates the proper NeoPixelBus type based on user settings for LED chipset, color order and pins.
   * Additional LED types can be added here and in the .cpp file.
   */
  class NeoPixelBusFactory {
    public:
      static INeoPixelBus *create(Chipset chipset, ColorOrder colorOrder, uint16_t ledCount);

    private:
      static INeoPixelBus *createWs2812Controller(Chipset chipset, ColorOrder colorOrder, uint16_t ledCount);
      static INeoPixelBus *createWs2813Controller(Chipset chipset, ColorOrder colorOrder, uint16_t ledCount);
  };
}}