#include "Rain.h"

#include "System/Clock.h"
#include "Algorithm/Waveform.h"

using namespace System;
using namespace Algorithm;

namespace Animations { namespace Spatial {

  void Rain::getFields(Web::UI::FieldsInfo& fields) {
    fields.addSlider("speed", "Speed", 5, 60);
  }

  void Rain::getSettings(JsonObject& settings) const {
    settings["speed"] = speed;
  }

  void Rain::setSettings(const JsonObject& settings) {
    if (settings["speed"]) {
      speed = settings["speed"];
    }
  }

  void Rain::renderFrame() {
    if (waveLeds != context.getRenderLedCount()) {
      waveLeds = context.getRenderLedCount();
      waveBase = (sqrt(waveLeds) / 2);
    }
    t1 = speed*(Clock::time(.25)+0.02);
    w = waveBase + (0.5*Clock::time(.3));

    SpatialAnimation::renderFrame();
  }

  void Rain::renderLed(int index, const Coordinate& coordinate) {
    float x = (float)coordinate.x / CoordinateMax;
    float y = (float)coordinate.y / CoordinateMax;

    float v = fmod(y-t1, Waveform::sine(x*w)); 
    v = v * v;

    byte sb = (1-((v > 0.8)/12)) * 255;
    byte vb = v * 255;

    // TODO: Fix after merging with neopixelbus branch
    //context.hsv(hue, sb, vb);
  }
}}