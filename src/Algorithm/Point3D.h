#pragma once

#include "Algorithm/Coordinate.h"

namespace Algorithm {
  /**
   * Represents a 3 dimensional coordinate
   */
  class Point3D {
    public:
      Point3D(): x(0), y(0), z(0) {}
      Point3D(CoordinateType x, CoordinateType y, CoordinateType z) : x(x), y(y), z(z) {}
      CoordinateType x, y, z;
  };
}