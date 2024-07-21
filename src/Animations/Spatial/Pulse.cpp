#include "Pulse.h"

#include "System/Clock.h"
#include "Algorithm/Math.h"
#include "Algorithm/Waveform.h"

using namespace System;
using namespace Algorithm;

namespace Animations { namespace Spatial {

  void Pulse::getFields(Web::UI::FieldsInfo& fields) {
    fields.addSlider("speed", "Speed", 1, 60, nullptr, 0);
  }

  void Pulse::getSettings(JsonObject& settings) const {
    settings["speed"] = (60 / speed) / 65.535;
  }

  void Pulse::setSettings(const JsonObject& settings) {
    if (settings["speed"]) {
      speed = (60 / (float)settings["speed"]) / 65.535;
    }
  }

  void Pulse::renderFrame() {
    t1 = Clock::time(speed);
    t1_wave = Waveform::sine(t1);
    r1 = sin(Clock::time(.1)*PI2);
    r2 = sin(Clock::time(.05)*PI2);
    r3 = sin(Clock::time(.07)*PI2);
    
    SpatialAnimation::renderFrame();
  }

  void Pulse::renderLed(int index, const Coordinate& coordinate) {
    float x= (float)coordinate.x / CoordinateMax;
    float y = (float)coordinate.y / CoordinateMax;
    float z = (float)coordinate.z / CoordinateMax;

    float wave = Math::fraction(3*t1_wave + x*r1 + y*r2 + z*r3);
    float v = Waveform::triangle(wave);
    v = v*v*v*v*v;

    float h = t1;
    float s = v < .8 ? 1 : 0;
    context.hsv(CHSV(255 * h, 255 * s, 255 * v));
  }
}}