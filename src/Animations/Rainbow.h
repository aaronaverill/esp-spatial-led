#pragma once
#include "Animation.h"

#include "Services/ILedDriverAnimationContext.h"

#include <FastLED.h>

namespace Animations {
  class Rainbow: public Animation {
    public:
      Rainbow(Services::ILedDriverAnimationContext& context);
      void getFields(Web::UI::FieldsInfo& fields);
      void getSettings(JsonObject& settings);
      void setSettings(const JsonObject& settings);
      void renderFrame();
      void renderLed(int index);
    private:
      void setBpm(byte bpm);
      byte bpm = 10;
      uint32_t everyMillis;
      byte hueIncrement;
      byte hue;
      byte repeat6 = 6; // Number of times the rainbow repeats in 1/6ths
  };
}