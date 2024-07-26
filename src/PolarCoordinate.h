#pragma once

#include <Arduino.h>
#include "Coordinate.h"
#include "Algorithm/Math.h"

#define PolarCoordinateType uint16_t
#define PolarCoordinateMax 65535

/**
 * Represents a polar coordinate (angle, distance)
 */
class PolarCoordinate {
  public:
    PolarCoordinate(): angle(0), distance(0) {}
    PolarCoordinate(PolarCoordinateType angle, PolarCoordinateType distance): angle(angle), distance(distance) {}
    PolarCoordinate(const Coordinate& coordinate) {
      // Scale the coordinates to the maximum value range when x = y = CoordinateMax (sqrt(1))
      float x = ((float)coordinate.x - (float)CoordinateMax / 2) * 0.7071067f;
      float y = ((float)coordinate.y - (float)CoordinateMax / 2) * 0.7071067f;
      distance = sqrt(x*x+y*y);
      angle = (0.5f + atan2(y, x) / PI2) * CoordinateMax;
    }
    PolarCoordinateType angle, distance;
};