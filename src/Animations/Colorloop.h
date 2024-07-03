#pragma once
#include "Animation.h"

#include "Services/ILedDriverAnimationContext.h"

#include <FastLED.h>

namespace Animations {
  class Colorloop: public Animation {
    public:
      Colorloop(Services::ILedDriverAnimationContext& context): Animation(context, "Colorloop") {}
      void render(int index);
    private:
      byte bpm = 10;
      byte hue;
  };
}