#include "Rainbow.h"

#include <FastLED.h>

namespace Animations {
  Rainbow::Rainbow(Services::ILedDriverAnimationContext& context): Animation(context, "Rainbow") {
    beat.setBpm(bpm);
  }

  void Rainbow::getFields(Web::UI::FieldsInfo& fields) {
    fields.addSlider("speed", "Speed", 1, 60, "`${val} bpm`", 0);
    fields.addSlider("repeat", "Repeat", 1, 5*6,
      "switch(parseInt(val)){case 1:case 5:val+'/6';break;case 2:case 4:(val/2)+'/3';break;case 3:'1/2';break;default:Math.round(val/6);}");
  }

  void Rainbow::getSettings(JsonObject& settings) const {
    settings["speed"] = bpm;
    settings["repeat"] = repeat6;
  }

  void Rainbow::setSettings(const JsonObject& settings) {
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

  void Rainbow::renderFrame() {
    beat.tick();
    Animation::renderFrame();
  }

  void Rainbow::renderLed(int index) {
    byte offset = (float)index*256/context.getRenderLedCount()*repeat6/6;
    context.hsv(beat.value+offset, 255, 255);
  }
}