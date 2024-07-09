#include "Rainbowfall.h"

namespace Animations { namespace Spatial {
  void Rainbowfall::render(int index) {
    if (index == 0) {
      hue = beat8(bpm);
    }
    const Coordinate& coordinate = context.getLedCoordinate(index);
    byte offset = spread*coordinate.x*255;
    context.hsv(CHSV(hue+offset, 255, 255));
  }
}}