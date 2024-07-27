#pragma once
#include "Animations/Polar/PolarAnimation.h"
//#include "PolarCoordinate.h"

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

      void renderLed(int index, const Algorithm::Point3D& point, const Algorithm::PointPolar& pointPolar) {
        byte hue = pointPolar.angle >> 8;
        byte saturation = min(255, 300 - (point.z >> 8));
        byte value = min(255, 127 + (pointPolar.distance >> 7));
        context.hsv(hue + t1, saturation, value);
      }

      byte t1;
      unsigned long lastTime = 0;
  };
}}