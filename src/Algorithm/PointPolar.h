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
      PointPolar(const Algorithm::Point3D& point) {
        // Scale the coordinates to the maximum value range when x = y = coordinate maximum (sqrt(1))
        float x = (Point3D::coordinatePercent(point.x) - 0.5) * 0.7071067f;
        float y = (Point3D::coordinatePercent(point.y) - 0.5) * 0.7071067f;
        distance = Point3D::percentToCoordinate(sqrt(x*x+y*y));
        angle = Point3D::percentToCoordinate(0.5f + atan2(y, x) / PI2);
      }
      CoordinateType angle, distance;
  };
}