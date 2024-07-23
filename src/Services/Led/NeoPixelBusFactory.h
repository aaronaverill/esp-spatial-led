#pragma once

#include "Services/Led/INeoPixelBus.h"

namespace Services { namespace Led {
  class NeoPixelBusFactory {
    public:
      static INeoPixelBus *create(Chipset chipset, ColorOrder colorOrder, uint16_t ledCount);

    private:
      static INeoPixelBus *createWs2812Controller(Chipset chipset, ColorOrder colorOrder, uint16_t ledCount);
      static INeoPixelBus *createWs2813Controller(Chipset chipset, ColorOrder colorOrder, uint16_t ledCount);
  };
}}