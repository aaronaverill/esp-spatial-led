#include "Colorloop.h"

#include <FastLED.h>

namespace Animations {
  Colorloop::Colorloop(Services::ILedDriverAnimationContext& context): Animation(context, "Colorloop") {
    beat.setBpm(bpm);
  }

  void Colorloop::getFields(Web::UI::FieldsInfo& fields) {
    fields.addSlider("speed", "Speed", 1, 60, "`${val} bpm`", 0);
  }

  void Colorloop::getSettings(JsonObject& settings) const {
    settings["speed"] = bpm;
  }

  void Colorloop::setSettings(const JsonObject& settings) {
    if (settings["speed"]) {
      bpm = settings["speed"];
      beat.setBpm(bpm);
    }
  }

  void Colorloop::renderFrame() {
    beat.tick();
    Animation::renderFrame();
  }

  void Colorloop::renderLed(int index) {
    context.hsv(beat.value,255,255);
  }
}