#include "Binary.h"

#include "System/Clock.h"
#include "Algorithm/Math.h"
#include "Algorithm/Waveform.h"

using namespace System;
using namespace Algorithm;

namespace Animations { namespace Spatial {

  void Binary::getFields(Web::UI::FieldsInfo& fields) {
    fields.addSlider("speed", "Speed", 1, 4, nullptr, 0);
  }

  void Binary::getSettings(JsonObject& settings) const {
    settings["speed"] = (60 / speed) / 65.535;
  }

  void Binary::setSettings(const JsonObject& settings) {
    if (settings["speed"]) {
      speed = (60 / (float)settings["speed"]) / 65.535;
    }
  }

  void Binary::renderFrame() {
    t1_saw = Clock::time(.1*speed);
    float t2 = Clock::time(0.1*speed) * PI2;
    float t3 = Clock::time(.5*speed);
    float t4 = Clock::time(0.2*speed) * PI2;

    t2_sin = sin(t2);
    t3_triangle = Waveform::triangle(t3);
    t4_sin = sin(t4);

    // Oscilate from [0.3 to 0.5]
    t5_triangle = 0.3 + Waveform::triangle(t1_saw) * 0.2;
    
    SpatialAnimation::renderFrame();
  }

  void Binary::renderLed(int index, const Coordinate& coordinate) {
    float x = (float)coordinate.x * .00001525;
    float y = (float)coordinate.y * .00001525;
    float z = (float)coordinate.z * .00001525;

    // Value range from [-0.08 to 0.06]
    float xyzGradient = (float)((int)(5*(x - 0.5)) ^ (int)(5 * (y - 0.5)) ^ (int)(5 * (z - 0.5))) * 0.02;

    float wave = Math::fraction((xyzGradient * (t3_triangle * 10 + 4 * t4_sin)) / t5_triangle);
    float h = t2_sin + Waveform::sine(wave);

    float v = Math::fraction(fabsf(h) + fabsf(t5_triangle) + t1_saw);
    v = Waveform::triangle(v*v);
    h = Math::fraction(Waveform::triangle(h) * 0.2 + (x + y + z) * 0.333333 + t1_saw);
    v = v * v * v;

    context.hsv(255 * h, 255, 255 * v);
  }
}}