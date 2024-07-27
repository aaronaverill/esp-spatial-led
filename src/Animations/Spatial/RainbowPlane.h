#pragma once
#include "Animations/Spatial/SpatialAnimation.h"
#include "Services/ILedDriverAnimationContext.h"
#include "Animations/Beat.h"

namespace Animations { namespace Spatial {
  class RainbowPlane: public SpatialAnimation {
    public:
      RainbowPlane(Services::ILedDriverAnimationContext& context);
      void getFields(Web::UI::FieldsInfo& fields);
      void getSettings(JsonObject& settings) const;
      void setSettings(const JsonObject& settings);
      void renderFrame();
      void renderLed(int index, const Coordinate& coordinate);
    private:
      byte bpm = 10;
      Beat beat;
      byte repeat6 = 6; // Number of times the rainbow repeats in 1/6ths
  };
}}