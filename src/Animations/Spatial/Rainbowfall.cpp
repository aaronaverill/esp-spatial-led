#include "Rainbowfall.h"

namespace Animations { namespace Spatial {
  void Rainbowfall::render(int index) {
    if (index == 0) {
      hue = beat8(bpm);
    }
    Coordinate& coordinate = context.getLedCoordinate(index);
    byte offset = spread*coordinate.z*255;
    context.hsv(CHSV(hue+offset, 255, 255));
  }
}}