#include "CubeFire.h"

#include "System/Clock.h"
#include "Algorithm/Waveform.h"

using namespace Algorithm;
using namespace System;

namespace Animations { namespace Spatial {
  void CubeFire::renderFrame() {
    t1 = Clock::time(.1/speed);
    float t2 = Clock::time(.13/speed);
    float t3 = Clock::time(.085/speed);

    t1_wave = Waveform::sine(t1);
    t2_wave = Waveform::sine(t2);
    t3_wave = Waveform::sine(t3);

    scale = (.5 + Waveform::sine(Clock::time(.1)))/2;

    SpatialAnimation::renderFrame();
  }

  void CubeFire::renderLed(int index, const Algorithm::Point3D& point) {
    float x = Point3D::coordinatePercent(point.x);
    float y = Point3D::coordinatePercent(point.y);
    float z = Point3D::coordinatePercent(point.z);

    float h = t1 + x*.25 + y*.25 + z*.25;
    float v = (Waveform::sine(z*scale + t1_wave) * Waveform::sine(y*scale + t2_wave) * Waveform::sine(x*scale + t3_wave))*10;
    float s = Math::fraction(v-1);
    v = v*v*v;
  
    h = Math::fraction(h);
    context.hsv(255 * h, 255 * s, 255 * v);
  }
}}