#ifndef SOFTRENDERER_INCLUDE_AABB_H_
#define SOFTRENDERER_INCLUDE_AABB_H_

#include <iostream>
#include "vector.h"

class AABB {
 public:
  AABB();
  AABB(const Vector4d &min, const Vector4d &max);
  ~AABB() = default;

  static AABB Union(const AABB &lhs, const AABB &rhs);
  static AABB Union(const AABB &lhs, const Vector4d &p);

  Vector4d min() const { return min_; }
  Vector4d max() const { return max_; }

 private:
  Vector4d min_, max_;
};

inline std::ostream &operator<<(std::ostream &os, const AABB &aabb) {
  os << "min: (" << aabb.min().x << "," << aabb.min().y << "," << aabb.min().z << ")" << std::endl;
  os << "max: (" << aabb.max().x << "," << aabb.max().y << "," << aabb.max().z << ")" << std::endl;
  return os;
}

#endif //SOFTRENDERER_INCLUDE_AABB_H_
