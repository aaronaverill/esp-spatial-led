#include "Pulse.h"

#include "System/Clock.h"
#include "Algorithm/Waveform.h"

using namespace System;
using namespace Algorithm;

namespace Animations { namespace Spatial {

  void Pulse::getFields(Web::UI::FieldsInfo& fields) {
    fields.addSlider("speed", "Speed", 1, 60);
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

    float intPart;
    float wave = 3*t1_wave + x*r1 + y*r2 + z*r3;
    wave = modff(wave, &intPart);

    float v = Waveform::triangle(wave);
    v = v*v*v*v*v;

    byte hb = min((float)255, 255 * t1);
    byte sb = v < .8 ? 255 : 0;
    byte vb = min((float)255, 255 * v);
    context.hsv(CHSV(hb, sb, vb));
  }
}}