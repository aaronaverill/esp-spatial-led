#pragma once
#include "Animations/Spatial/SpatialAnimation.h"
#include "Services/ILedDriverAnimationContext.h"
#include "Algorithm/Matrix3D.h"

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
      void setBpm(byte bpm);
      void updateRotation();
      byte bpm = 10;
      uint32_t everyMillis;
      byte hueIncrement;
      byte hue;
      byte repeat6 = 6; // Number of times the rainbow repeats in 1/6ths
      CoordinateType rotation[3] = {0, 0, 0};
      Algorithm::Matrix3D rotationMatrix;
  };
}}