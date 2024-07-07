#pragma once
#include "Animation.h"

#include "Services/ILedDriverAnimationContext.h"

namespace Animations {
  class Solid: public Animation {
    public:
      Solid(Services::ILedDriverAnimationContext& context): Animation(context, "Solid") {}
      void getFields(JsonArray& fields);
      void getSettings(JsonObject& settings);
      void setSettings(std::vector<KeyValuePair> values);
      void render(int index);
    private:
      CHSV hsv = CHSV(196, 255, 255);
  };
}