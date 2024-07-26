#pragma once

#include <math.h>

namespace Algorithm {
  class Vector3 {
    public:
      float x, y, z;

      Vector3(float x, float y, float z) : x(x), y(y), z(z) {}

      float dot(const Vector3& other) const {
        return x * other.x + y * other.y + z * other.z;
      }

      Vector3 operator*(float scalar) const {
        return Vector3(x * scalar, y * scalar, z * scalar);
      }

      Vector3 operator+(const Vector3& other) const {
        return Vector3(x + other.x, y + other.y, z + other.z);
      }

      Vector3 operator-(const Vector3& other) const {
        return Vector3(x - other.x, y - other.y, z - other.z);
      }

      void normalize() {
        double magnitude = sqrt(x * x + y * y + z * z);
        x /= magnitude;
        y /= magnitude;
        z /= magnitude;
      }
  };
}