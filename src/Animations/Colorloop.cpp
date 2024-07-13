#include "Colorloop.h"

#include <FastLED.h>

namespace Animations {
  Colorloop::Colorloop(Services::ILedDriverAnimationContext& context): Animation(context, "Colorloop") {
    setBpm(bpm);
  }

  void Colorloop::getFields(Web::UI::FieldsInfo& fields) {
    fields.addSlider("speed", "Speed", 1, 60, "`${val} bpm`", 0);
  }

  void Colorloop::getSettings(JsonObject& settings) {
    settings["speed"] = bpm;
  }

  void Colorloop::setSettings(const JsonObject& settings) {
    if (settings["speed"]) {
      setBpm(settings["speed"]);
    }
  }

  void Colorloop::renderFrame() {
    EVERY_N_MILLIS_I(beat,everyMillis) {
      hue += hueIncrement;
    }
    Animation::renderFrame();
  }

  void Colorloop::renderLed(int index) {
    context.hsv(CHSV(hue,255,255));
  }

  void Colorloop::setBpm(byte bpm) {
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
}