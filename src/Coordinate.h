#pragma once

#include <Arduino.h>
#define CoordinateType uint16_t
#define CoordinateMax 65535

/**
 * Represents a 3 dimensional coordinate
 */
class Coordinate {
  public:
    Coordinate(): x(0), y(0), z(0) {}
    Coordinate(CoordinateType x, CoordinateType y, CoordinateType z) : x(x), y(y), z(z) {}
    CoordinateType x, y, z;
};