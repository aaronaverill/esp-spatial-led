#pragma once
#include "Animations/Animation.h"
#include "Algorithm/PointPolar.h"

namespace Animations { namespace Polar {
  class PolarAnimation: public Animation {
    public:
      /**
       * Construct the animation with a context and name
       */
      PolarAnimation(Services::ILedDriverAnimationContext& context, const char *name, const char *tags = "P") : Animation(context, name, tags) {}

      /**
       * Render a specific led by index
       */
      void renderLed(int index) {
        const Algorithm::Point3D& point = context.getLedCoordinate(index);
        const Algorithm::PointPolar& pointPolar = context.getLedPolarCoordinate(index);
        renderLed(index, point, pointPolar);
      }

      virtual void renderLed(int index, const Algorithm::Point3D& point, const Algorithm::PointPolar& pointPolar) {}
  };
}}