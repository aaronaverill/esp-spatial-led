#include "CubeFire.h"

#include "System/Clock.h"
#include "Algorithm/Waveform.h"

using namespace System;
using namespace Algorithm;

namespace Animations { namespace Spatial {
  void CubeFire::renderFrame() {
    t1 = Clock::time(.1/speed);
    t2 = Clock::time(.13/speed);
    t3 = Clock::time(.085/speed);
    scale = (.5 + Waveform::sine(Clock::time(.1)))/2;

    t1_wave = Waveform::sine(t1);
    t2_wave = Waveform::sine(t2);
    t3_wave = Waveform::sine(t3);

    SpatialAnimation::renderFrame();
  }

  void CubeFire::renderLed(int index, const Coordinate& coordinate) {
    float x = (float)coordinate.x / CoordinateMax;
    float y = (float)coordinate.y / CoordinateMax;
    float z = (float)coordinate.z / CoordinateMax;

    float h = t1 + x*.25 + y*.25 + z*.25;
    float s = 1;
    float v = (Waveform::sine(z*scale + t1_wave) * Waveform::sine(y*scale + t2_wave) * Waveform::sine(x*scale + t3_wave))*10;
    s = v-1;
    v = v*v*v;
  
    if (v < 0.05) {
      context.rgb(CRGB(0,0,0));
    } else {
      byte hb = min((float)255,h*255);
      byte sb = min((float)255,s*255);
      byte vb = min((float)255,v*255);
      context.hsv(CHSV(hb,sb,vb));
    }
  }
}}