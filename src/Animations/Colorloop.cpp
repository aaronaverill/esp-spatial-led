#include "Colorloop.h"

#include <FastLED.h>

namespace Animations {
  void Colorloop::render(int index) {
    if (index == 0) {
      hue = beat8(bpm);
    }
    context.hsv(CHSV(hue,255,255));
  }
}