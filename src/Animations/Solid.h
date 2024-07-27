#pragma once
#include "Animation.h"

#include "Services/ILedDriverAnimationContext.h"
#include "Color.h"

namespace Animations {
  class Solid: public Animation {
    public:
      Solid(Services::ILedDriverAnimationContext& context): Animation(context, "Solid") { color.rgb.r = 255; }
      void getFields(Web::UI::FieldsInfo& fields);
      void getSettings(JsonObject& settings) const;
      void setSettings(const JsonObject& settings);
      void renderLed(int index);
    private:
      Color color;
  };
}