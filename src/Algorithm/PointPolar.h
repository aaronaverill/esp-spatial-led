#pragma once

#include <Arduino.h>
#include "Algorithm/Coordinate.h"
#include "Algorithm/Math.h"
#include "Algorithm/Point3D.h"

namespace Algorithm {
  /**
   * Represents a polar coordinate (angle, distance)
   */
  class PointPolar {
    public:
      PointPolar(): angle(0), distance(0) {}
      PointPolar(CoordinateType angle, CoordinateType distance): angle(angle), distance(distance) {}
      PointPolar(const Algorithm::Point3D& coordinate) {
        // Scale the coordinates to the maximum value range when x = y = CoordinateMax (sqrt(1))
        float x = ((float)coordinate.x - (float)CoordinateMax / 2) * 0.7071067f;
        float y = ((float)coordinate.y - (float)CoordinateMax / 2) * 0.7071067f;
        distance = sqrt(x*x+y*y);
        angle = (0.5f + atan2(y, x) / PI2) * CoordinateMax;
      }
      CoordinateType angle, distance;
  };
}