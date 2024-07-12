#pragma once
#include "Animation.h"

#include "Services/ILedDriverAnimationContext.h"

#include <FastLED.h>

namespace Animations {
  class Rainbow: public Animation {
    public:
      Rainbow(Services::ILedDriverAnimationContext& context): Animation(context, "Rainbow") {}
      void renderLed(int index);
    private:
      uint16_t bpm = 10;
      uint8_t size = 5;
      byte hue;
  };
}