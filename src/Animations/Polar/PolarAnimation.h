#pragma once
#include "Animations/Animation.h"
#include "PolarCoordinate.h"

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
        const Coordinate& coordinate = context.getLedCoordinate(index);
        const PolarCoordinate& polar = context.getLedPolarCoordinate(index);
        renderLed(index, coordinate, polar);
      }

      virtual void renderLed(int index, const Coordinate& coordinate, const PolarCoordinate& polar) {}
  };
}}