#pragma once
#include "Animations/Spatial/SpatialAnimation.h"

#include "Services/ILedDriverAnimationContext.h"

namespace Animations { namespace Spatial {
  class Binary: public SpatialAnimation {
    public:
      Binary(Services::ILedDriverAnimationContext& context): SpatialAnimation(context, "Binary") {}
      void getFields(Web::UI::FieldsInfo& fields);
      void getSettings(JsonObject& settings) const;
      void setSettings(const JsonObject& settings);
      void renderFrame();
      void renderLed(int index, const Coordinate& coordinate);
    private:
      float speed = 6;
      float t1_saw;
      float t2_sin;
      float t3_triangle;
      float t4_sin;
      float t5_triangle;
  };
}}