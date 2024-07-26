#pragma once
#include "Animations/Spatial/SpatialAnimation.h"

#include "Services/ILedDriverAnimationContext.h"
#include "Algorithm/Quaternion.h"

namespace Animations { namespace Spatial {
  class Test: public SpatialAnimation {
    public:
      Test(Services::ILedDriverAnimationContext& context);
      void getFields(Web::UI::FieldsInfo& fields);
      void getSettings(JsonObject& settings) const;
      void setSettings(const JsonObject& settings);
      void renderFrame();
      void renderLed(int index, const Coordinate& coordinate);
    private:
      void generateRandomVector();

      float speed = 6;

      Algorithm::Quaternion quaternion;
      float translationDistance = 0;
      float directionSpeed = -0.2;
  };
}}