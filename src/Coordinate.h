#pragma once

#define CoordinateType float

class Coordinate {
  public:
    Coordinate(): x(0), y(0), z(0) {}
    Coordinate(CoordinateType x, CoordinateType y, CoordinateType z) : x(x), y(y), z(z) {}
    CoordinateType x, y, z;
};