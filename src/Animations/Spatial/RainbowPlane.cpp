#include "RainbowPlane.h"

#include <FastLED.h>

namespace Animations { namespace Spatial {
  RainbowPlane::RainbowPlane(Services::ILedDriverAnimationContext& context):
    SpatialAnimation(context, "Rainbow Plane") {
    setBpm(bpm);
  }

  void RainbowPlane::getFields(Web::UI::FieldsInfo& fields) {
    fields.addSlider("speed", "Speed", 1, 60, "`${val} bpm`", 0);
    fields.addSlider("repeat", "Repeat", 1, 5*6,
      "switch(parseInt(val)){case 1:case 5:val+'/6';break;case 2:case 4:(val/2)+'/3';break;case 3:'1/2';break;default:Math.round(val/6);}");
  }

  void RainbowPlane::getSettings(JsonObject& settings) const {
    settings["speed"] = bpm;
    settings["repeat"] = repeat6;
  }

  void RainbowPlane::setSettings(const JsonObject& settings) {
    if (settings["speed"]) {
      setBpm(settings["speed"]);
    }
    if (settings["repeat"]) {
      repeat6 = settings["repeat"];
      if (repeat6 > 6) {
        repeat6 = round((float)repeat6/6)*6;
      }
    }
  }

  void RainbowPlane::renderFrame() {
    EVERY_N_MILLIS_I(beat,everyMillis) {
      hue += hueIncrement;
    }
    Animation::renderFrame();
  }


  void RainbowPlane::renderLed(int index, const Coordinate& coordinate) {
    byte offset = 255.f * ((float) coordinate.z / CoordinateMax) * ((float)repeat6 / 6.f);
    context.hsv(hue+offset, 255, 255);
  }

  void RainbowPlane::setBpm(byte bpm) {
    this->bpm = bpm;
    hueIncrement = 1;
    float timeBetween = 234.375/bpm;
    // Don't update more than once every 20 milliseconds
    if (timeBetween < 20) {
      hueIncrement = 20/timeBetween;
      timeBetween *= hueIncrement;
    }
    everyMillis = timeBetween;
  }
}}