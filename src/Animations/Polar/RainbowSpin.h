#pragma once
#include "Animations/Polar/PolarAnimation.h"
#include "PolarCoordinate.h"

namespace Animations { namespace Polar {
  class RainbowSpin: public PolarAnimation {
    public:
      /**
       * Construct the animation with a context and name
       */
      RainbowSpin(Services::ILedDriverAnimationContext& context) : PolarAnimation(context, "Rainbow Spin") {}

      void renderFrame() {
        unsigned long nowTime = millis();
        if (nowTime - lastTime > 20) {
          t1++;
          lastTime = nowTime;
        }
        PolarAnimation::renderFrame();
      }

      void renderLed(int index, const Coordinate& coordinate, const PolarCoordinate& polar) {
        byte hue = polar.angle >> 8;
        byte saturation = min(255, 300 - (coordinate.z >> 8));
        byte value = min(255, 127 + (polar.distance >> 7));
        context.hsv(hue + t1, saturation, value);
      }

      byte t1;
      unsigned long lastTime = 0;
  };
}}