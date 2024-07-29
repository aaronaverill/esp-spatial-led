#pragma once
#include "Animation.h"

#include "Services/ILedDriverAnimationContext.h"
#include "Animations/Beat.h"

namespace Animations {
  class RainbowFade: public Animation {
    public:
      RainbowFade(Services::ILedDriverAnimationContext& context);
      void getFields(Web::UI::FieldsInfo& fields);
      void getSettings(JsonObject& settings) const;
      void setSettings(const JsonObject& settings);
      void renderFrame();
      void renderLed(int index);
    private:
      uint8_t bpm = 10;
      Beat beat;
  };
}