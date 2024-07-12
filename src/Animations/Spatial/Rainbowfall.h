#pragma once
#include "Animations/Animation.h"

#include "Services/ILedDriverAnimationContext.h"

namespace Animations { namespace Spatial {
  class Rainbowfall: public Animation {
    public:
      Rainbowfall(Services::ILedDriverAnimationContext& context): Animation(context, "Rainbowfall") {}
      void renderFrame();
      void renderLed(int index);
    private:
      uint16_t bpm = 10;
      float spread = 0.3;
      byte hue;
  };
}}