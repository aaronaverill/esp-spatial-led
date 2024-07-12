#pragma once
#include "Animation.h"

#include "Services/ILedDriverAnimationContext.h"

namespace Animations {
  class Solid: public Animation {
    public:
      Solid(Services::ILedDriverAnimationContext& context): Animation(context, "Solid") {}
      void getFields(Web::UI::FieldsInfo& fields);
      void getSettings(JsonObject& settings);
      void setSettings(std::vector<KeyValuePair<String, String>> values);
      void renderFrame();
      void renderLed(int index);
    private:
      byte setHue = 196;
      CHSV hsv = CHSV(196, 255, 255);
  };
}