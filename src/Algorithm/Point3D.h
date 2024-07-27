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

      /**
       * Get the percent value of a coordinate as a float from [0..1]
       */
      static float coordinatePercent(CoordinateType v) {
        // TODO: This only works if CoordinateType is uint16_t. 
        // If we offer different coordinate type this will need to be updated
        return 1.525902e-5 * (float)v; // Approximation of 1/65535 that doesnt' use slow floating point division
      }

      /**
       * Convert a percent from [0..1] to a coordinate value 
       */
      static CoordinateType percentToCoordinate(float percent) {
        // TODO: This only works if CoordinateType is uint16_t.
        // If we offer different coordinate type this will need to be updated
        return percent * 65535.f;
      }

      CoordinateType x, y, z;
  };
}