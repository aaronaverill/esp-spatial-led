#include "Matrix3D.h"
#include <Arduino.h>

namespace Algorithm {
  Matrix3D::Matrix3D() {
    for (int i = 0; i < 4; ++i) {
      for (int j = 0; j < 4; ++j) {
        matrix[i][j] = (i == j) ? 1.0f : 0.0f;
      }
    }
  }

  Matrix3D Matrix3D::identity() {
    return Matrix3D();
  }

  Matrix3D Matrix3D::translation(CoordinateType tx, CoordinateType ty, CoordinateType tz) {
    Matrix3D result;
    result.matrix[0][3] = tx;
    result.matrix[1][3] = ty;
    result.matrix[2][3] = tz;
    return result;
  }

  Matrix3D Matrix3D::rotation(CoordinateType ax, CoordinateType ay, CoordinateType az) {
    float pi2 = 6.28318530718;
    Matrix3D result;
    CoordinateType cx = std::cos(((float)ax)/CoordinateMax * pi2);
    CoordinateType sx = std::sin(((float)ax)/CoordinateMax * pi2);
    CoordinateType cy = std::cos(((float)ay)/CoordinateMax * pi2);
    CoordinateType sy = std::sin(((float)ay)/CoordinateMax * pi2);
    CoordinateType cz = std::cos(((float)az)/CoordinateMax * pi2);
    CoordinateType sz = std::sin(((float)az)/CoordinateMax * pi2);

    result.matrix[0][0] = cy * cz;
    result.matrix[0][1] = -cy * sz;
    result.matrix[0][2] = sy;
    result.matrix[0][3] = 0;

    result.matrix[1][0] = sx * sy * cz + cx * sz;
    result.matrix[1][1] = -sx * sy * sz + cx * cz;
    result.matrix[1][2] = -sx * cy;
    result.matrix[1][3] = 0;

    result.matrix[2][0] = -cx * sy * cz + sx * sz;
    result.matrix[2][1] = cx * sy * sz + sx * cz;
    result.matrix[2][2] = cx * cy;
    result.matrix[2][3] = 0;

    result.matrix[3][0] = 0;
    result.matrix[3][1] = 0;
    result.matrix[3][2] = 0;
    result.matrix[3][3] = 1;

    return result;
  }

/*
  Matrix3D Matrix3D::rotationX(CoordinateType angle) {
    Matrix3D result;
    CoordinateType c = std::cos(angle);
    CoordinateType s = std::sin(angle);
    result.matrix[1][1] = c;
    result.matrix[1][2] = -s;
    result.matrix[2][1] = s;
    result.matrix[2][2] = c;
    return result;
  }

  Matrix3D Matrix3D::rotationY(CoordinateType angle) {
    Matrix3D result;
    CoordinateType c = std::cos(angle);
    CoordinateType s = std::sin(angle);
    result.matrix[0][0] = c;
    result.matrix[0][2] = s;
    result.matrix[2][0] = -s;
    result.matrix[2][2] = c;
    return result;
  }

  Matrix3D Matrix3D::rotationZ(CoordinateType angle) {
    Matrix3D result;
    CoordinateType c = std::cos(angle);
    CoordinateType s = std::sin(angle);
    result.matrix[0][0] = c;
    result.matrix[0][1] = -s;
    result.matrix[1][0] = s;
    result.matrix[1][1] = c;
    return result;
  }

  Matrix3D Matrix3D::scale(CoordinateType sx, CoordinateType sy, CoordinateType sz) {
    Matrix3D result;
    result.matrix[0][0] = sx;
    result.matrix[1][1] = sy;
    result.matrix[2][2] = sz;
    return result;
  }
*/

  Matrix3D Matrix3D::operator*(const Matrix3D& other) const {
    Matrix3D result;
    for (int i = 0; i < 4; ++i) {
      for (int j = 0; j < 4; ++j) {
        result.matrix[i][j] = 0;
        for (int k = 0; k < 4; ++k) {
          result.matrix[i][j] += matrix[i][k] * other.matrix[k][j];
        }
      }
    }
    return result;
  }

  void Matrix3D::multiplyVec(const CoordinateType vec[4], CoordinateType result[4]) const {
    for (int i = 0; i < 4; ++i) {
      result[i] = 0;
      for (int j = 0; j < 4; ++j) {
       result[i] += matrix[i][j] * vec[j];
      }
    }
  }
}