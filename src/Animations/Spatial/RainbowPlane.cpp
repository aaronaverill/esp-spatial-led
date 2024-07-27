#include "RainbowPlane.h"

#include <FastLED.h>

using namespace Algorithm;

namespace Animations { namespace Spatial {
  RainbowPlane::RainbowPlane(Services::ILedDriverAnimationContext& context):
    SpatialAnimation(context, "Rainbow Plane") {
    beat.setBpm(bpm);
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
      bpm = settings["speed"];
      beat.setBpm(bpm);
    }
    if (settings["repeat"]) {
      repeat6 = settings["repeat"];
      if (repeat6 > 6) {
        repeat6 = round((float)repeat6/6)*6;
      }
    }
  }

  void RainbowPlane::renderFrame() {
    beat.tick();
    Animation::renderFrame();
  }


  void RainbowPlane::renderLed(int index, const Point3D& point) {
    byte offset = 255.f * Point3D::coordinatePercent(point.z) * ((float)repeat6 / 6.f);
    context.hsv(beat.value+offset, 255, 255);
  }
}}