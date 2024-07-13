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

  Matrix3D Matrix3D::translation(float tx, float ty, float tz) {
    Matrix3D result;
    result.matrix[0][3] = tx;
    result.matrix[1][3] = ty;
    result.matrix[2][3] = tz;
    return result;
  }

  Matrix3D Matrix3D::rotation(float ax, float ay, float az) {
    Matrix3D result;
    float cx = std::cos(ax);
    float sx = std::sin(ax);
    float cy = std::cos(ay);
    float sy = std::sin(ay);
    float cz = std::cos(az);
    float sz = std::sin(az);

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

  Matrix3D Matrix3D::rotationX(float angle) {
    Matrix3D result;
    float c = std::cos(angle);
    float s = std::sin(angle);
    result.matrix[1][1] = c;
    result.matrix[1][2] = -s;
    result.matrix[2][1] = s;
    result.matrix[2][2] = c;
    return result;
  }

  Matrix3D Matrix3D::rotationY(float angle) {
    Matrix3D result;
    float c = std::cos(angle);
    float s = std::sin(angle);
    result.matrix[0][0] = c;
    result.matrix[0][2] = s;
    result.matrix[2][0] = -s;
    result.matrix[2][2] = c;
    return result;
  }

  Matrix3D Matrix3D::rotationZ(float angle) {
    Matrix3D result;
    float c = std::cos(angle);
    float s = std::sin(angle);
    result.matrix[0][0] = c;
    result.matrix[0][1] = -s;
    result.matrix[1][0] = s;
    result.matrix[1][1] = c;
    return result;
  }

  Matrix3D Matrix3D::scale(float sx, float sy, float sz) {
    Matrix3D result;
    result.matrix[0][0] = sx;
    result.matrix[1][1] = sy;
    result.matrix[2][2] = sz;
    return result;
  }

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

  void Matrix3D::multiplyVec(const float vec[4], float result[4]) const {
    for (int i = 0; i < 4; ++i) {
      result[i] = 0;
      for (int j = 0; j < 4; ++j) {
       result[i] += matrix[i][j] * vec[j];
      }
    }
  }
}