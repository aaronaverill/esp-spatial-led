#pragma once
#include "Animation.h"

#include "Services/ILedDriverAnimationContext.h"
#include "Animations/Beat.h"
#include "PaletteChoice.h"

namespace Animations {
  class RainbowChase: public Animation {
    public:
      RainbowChase(Services::ILedDriverAnimationContext& context);
      void getFields(Web::UI::FieldsInfo& fields);
      void getSettings(JsonObject& settings) const;
      void setSettings(const JsonObject& settings);
      void renderFrame();
      void renderLed(int index);
    private:
      uint8_t bpm = 10;
      Beat beat;
      uint8_t repeat6 = 6; // Number of times the rainbow repeats in 1/6ths
      PaletteChoice palette;
  };
}