#ifndef SOFTRENDERER_INCLUDE_MATH_UTIL_H_
#define SOFTRENDERER_INCLUDE_MATH_UTIL_H_

#include "vector.h"

const double kPI = 3.14159265358979323846;

inline double Radian(double angle) { return kPI * angle / 180.0; }

template <typename T>
inline void Clamp(Vector4<T> &vec, T min, T max) {
  vec.x = std::max(std::min(max, vec.x), min);
  vec.y = std::max(std::min(max, vec.y), min);
  vec.z = std::max(std::min(max, vec.z), min);
  vec.w = std::max(std::min(max, vec.w), min);
}

template <typename T>
inline void Clamp(T &num, T min, T max) {
  num = std::max(std::min(max, num), min);
}

#endif //SOFTRENDERER_INCLUDE_MATH_UTIL_H_
