#pragma once
#include "Animations/Spatial/SpatialAnimation.h"

#include "Services/ILedDriverAnimationContext.h"

namespace Animations { namespace Spatial {
  class Rain: public SpatialAnimation {
    public:
      Rain(Services::ILedDriverAnimationContext& context): SpatialAnimation(context, "Rain") {}
      void getFields(Web::UI::FieldsInfo& fields);
      void getSettings(JsonObject& settings) const;
      void setSettings(const JsonObject& settings);
      void renderFrame();
      void renderLed(int index, const Coordinate& coordinate);
    private:
      byte hue = 150;
      float speed = 10;

      float t1;
      float w;

      uint waveLeds = 0;
      float waveBase;
  };
}}