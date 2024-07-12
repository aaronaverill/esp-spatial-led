#include "Rainbowfall.h"

namespace Animations { namespace Spatial {
  void Rainbowfall::renderFrame() {
    hue = beat8(bpm);
    Animation::renderFrame();
  }

  void Rainbowfall::renderLed(int index) {
    const Coordinate& coordinate = context.getLedCoordinate(index);
    byte offset = spread*coordinate.x*255;
    context.hsv(CHSV(hue+offset, 255, 255));
  }
}}