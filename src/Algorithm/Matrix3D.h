#pragma once

namespace Algorithm {
  /**
   * Utility class to perform transformations on 3D points. Generated by Chat GPT.
   */
  class Matrix3D {
    public:
      float matrix[4][4];

      Matrix3D();
      static Matrix3D identity();
      static Matrix3D translation(float tx, float ty, float tz);
      static Matrix3D rotation(float ax, float ay, float az);
      static Matrix3D rotationX(float angle);
      static Matrix3D rotationY(float angle);
      static Matrix3D rotationZ(float angle);
      static Matrix3D scale(float sx, float sy, float sz);

      Matrix3D operator*(const Matrix3D& other) const;
      void multiplyVec(const float vec[4], float result[4]) const;
  };
}