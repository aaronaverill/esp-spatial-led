#include "Colorloop.h"

#include <FastLED.h>

namespace Animations {
  void Colorloop::renderFrame() {
    hue = beat8(bpm);
    Animation::renderFrame();
  }

  void Colorloop::renderLed(int index) {
    context.hsv(CHSV(hue,255,255));
  }
}