#pragma once
#include "Animations/Spatial/SpatialAnimation.h"

#include "Services/ILedDriverAnimationContext.h"

namespace Animations { namespace Spatial {
  class Pulse: public SpatialAnimation {
    public:
      Pulse(Services::ILedDriverAnimationContext& context): SpatialAnimation(context, "Pulse") {}
      void getFields(Web::UI::FieldsInfo& fields);
      void getSettings(JsonObject& settings) const;
      void setSettings(const JsonObject& settings);
      void renderFrame();
      void renderLed(int index, const Algorithm::Point3D& point);
    private:
      float speed = 60;
      float t1, t1_wave, r1, r2, r3;
  };
}}