#pragma once
#include "Animations/Animation.h"

#include "Services/ILedDriverAnimationContext.h"

namespace Animations { namespace Spatial {
  class Rainbowfall: public Animation {
    public:
      Rainbowfall(Services::ILedDriverAnimationContext& context);
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
      byte repeat6 = 18; // Number of times the rainbow repeats in 1/6ths
  };
}}