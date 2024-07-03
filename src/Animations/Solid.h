#pragma once
#include "Animation.h"

#include "Services/ILedDriverAnimationContext.h"

namespace Animations {
  class Solid: public Animation {
    public:
      Solid(Services::ILedDriverAnimationContext& context): Animation(context, "Solid") {}
      void render(int index);
    private:
      CRGB rgb = CRGB::Red;
  };
}