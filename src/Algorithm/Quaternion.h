#pragma once

#include "Vector.h"

namespace Algorithm {
  class Quaternion {
    public:
      float w, x, y, z;

      Quaternion(): w(1), x(0), y(0), z(0) {}

      Quaternion(float w, float x, float y, float z): w(w), x(x), y(y), z(z) {}

      void normalize() {
        float magnitude = sqrt(w * w + x * x + y * y + z * z);
        w /= magnitude;
        x /= magnitude;
        y /= magnitude;
        z /= magnitude;
      }

      Vector3 rotate(const Vector3& v) const {
        Quaternion qv(0, v.x, v.y, v.z);
        Quaternion qConjugate = conjugate();
        Quaternion result = (*this) * qv * qConjugate;

        return Vector3(result.x, result.y, result.z);
      }

      Quaternion operator*(const Quaternion& other) const {
        return Quaternion(
            w * other.w - x * other.x - y * other.y - z * other.z,
            w * other.x + x * other.w + y * other.z - z * other.y,
            w * other.y - x * other.z + y * other.w + z * other.x,
            w * other.z + x * other.y - y * other.x + z * other.w
        );
      }

      Vector3 getNormal() const {
        return rotate(Vector3(0, 0, 1));
      }

      Quaternion translate(float distance) const {
        Vector3 normal = getNormal();
        Vector3 translation = normal * distance;
        return Quaternion(w, x + translation.x, y + translation.y, z + translation.z);
      }

    private:
      Quaternion conjugate() const {
        return Quaternion(w, -x, -y, -z);
      }
  };
}