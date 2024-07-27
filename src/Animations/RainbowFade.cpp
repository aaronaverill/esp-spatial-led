#include "RainbowFade.h"

#include <FastLED.h>

namespace Animations {
  RainbowFade::RainbowFade(Services::ILedDriverAnimationContext& context): Animation(context, "Rainbow Fade") {
    beat.setBpm(bpm);
  }

  void RainbowFade::getFields(Web::UI::FieldsInfo& fields) {
    fields.addSlider("speed", "Speed", 1, 60, "`${val} bpm`", 0);
  }

  void RainbowFade::getSettings(JsonObject& settings) const {
    settings["speed"] = bpm;
  }

  void RainbowFade::setSettings(const JsonObject& settings) {
    if (settings["speed"]) {
      bpm = settings["speed"];
      beat.setBpm(bpm);
    }
  }

  void RainbowFade::renderFrame() {
    beat.tick();
    Animation::renderFrame();
  }

  void RainbowFade::renderLed(int index) {
    context.hsv(beat.value,255,255);
  }
}