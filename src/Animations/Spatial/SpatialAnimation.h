#pragma once
#include "Animations/Animation.h"

namespace Animations { namespace Spatial {
  class SpatialAnimation: public Animation {
    public:
      /**
       * Construct the animation with a context and name
       */
      SpatialAnimation(Services::ILedDriverAnimationContext& context, const char *name, const char *tags = "3") : Animation(context, name, tags) {}

      /**
       * Render a specific led by index
       */
      void renderLed(int index) {
        const Coordinate& coordinate = context.getLedCoordinate(index);
        renderLed(index, coordinate);
      }

      virtual void renderLed(int index, const Coordinate& coordinate) {}
  };
}}