#include "aabb.h"
#include <limits>

AABB::AABB() {
  min_ = Vector4d(std::numeric_limits<double>::max(),
				  std::numeric_limits<double>::max(),
				  std::numeric_limits<double>::max());
  max_ = Vector4d(std::numeric_limits<double>::lowest(),
				  std::numeric_limits<double>::lowest(),
				  std::numeric_limits<double>::lowest());
}

AABB::AABB(const Vector4d &p1, const Vector4d &p2) {
  min_ = Vector4d(std::fmin(p1.x, p2.x), std::fmin(p1.y, p2.y), std::fmin(p1.z, p2.z));
  max_ = Vector4d(std::fmax(p1.x, p2.x), std::fmax(p1.y, p2.y), std::fmax(p1.z, p2.z));
}

AABB AABB::Union(const AABB &lhs, const AABB &rhs) {
  Vector4d min, max;
  min = Vector4d(std::fmin(lhs.min_.x, rhs.min_.x),
				 std::fmin(lhs.min_.y, rhs.min_.y),
				 std::fmin(lhs.min_.z, rhs.min_.z));
  max = Vector4d(std::fmax(lhs.max_.x, rhs.max_.x),
				 std::fmax(lhs.max_.y, rhs.max_.y),
				 std::fmax(lhs.max_.z, rhs.max_.z));
  return AABB(min, max);
}

AABB AABB::Union(const AABB &lhs, const Vector4d &p) {
  Vector4d min, max;
  min = Vector4d(std::fmin(lhs.min_.x, p.x),
				 std::fmin(lhs.min_.y, p.y),
				 std::fmin(lhs.min_.z, p.z));
  max = Vector4d(std::fmax(lhs.max_.x, p.x),
				 std::fmax(lhs.max_.y, p.y),
				 std::fmax(lhs.max_.z, p.z));
  return AABB(min, max);
}