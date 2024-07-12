#include "Rainbow.h"

#include <FastLED.h>

namespace Animations {
  void Rainbow::renderLed(int index) {
    if (index == 0) {
      hue = beat8(bpm);
    }    
    context.hsv(CHSV(hue+index*size,255,255));
  }
}