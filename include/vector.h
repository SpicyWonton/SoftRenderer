#ifndef SOFTRENDERER_INCLUDE_VECTOR_H_
#define SOFTRENDERER_INCLUDE_VECTOR_H_

#include <cmath>

template <typename T>
class Vector4;

template <typename T>
class Vector3 {
 public:
  Vector3() : x(0), y(0), z(0) {}
  Vector3(T x_, T y_, T z_) : x(x_), y(y_), z(z_) {}
  Vector3(const Vector3 &rhs) : x(rhs.x), y(rhs.y), z(rhs.z) {}
  Vector3(const Vector4<T> &rhs) : x(rhs.x), y(rhs.y), z(rhs.z) {}
  ~Vector3() = default;

  Vector3 &operator=(const Vector4<T> &rhs) {
    x = rhs.x;
    y = rhs.y;
    z = rhs.z;
    return *this;
  }

  Vector3 operator+(const Vector3 &rhs) {
	T a = this->x + rhs.x;
	T b = this->y + rhs.y;
	T c = this->z + rhs.z;
	return Vector3(a, b, c);
  }
  Vector3 operator-(const Vector3 &rhs) const {
	T a = this->x - rhs.x;
	T b = this->y - rhs.y;
	T c = this->z - rhs.z;
	return Vector3(a, b, c);
  }
  Vector3 &operator+=(const Vector3 &rhs) {
	this->x += rhs.x;
	this->y += rhs.y;
	this->z += rhs.z;
	return *this;
  }
  Vector3 &operator-=(const Vector3 &rhs) {
	this->x -= rhs.x;
	this->y -= rhs.y;
	this->z -= rhs.z;
	return *this;
  }

  Vector3 operator*(T t) const {
	T a = this->x * t;
	T b = this->y * t;
	T c = this->z * t;
	return Vector3(a, b, c);
  }
  Vector3 operator/(T t) {
	T a = this->x / t;
	T b = this->y / t;
	T c = this->z / t;
	return Vector3(a, b, c);
  }
  Vector3 &operator*=(T t) {
	this->x *= t;
	this->y *= t;
	this->z *= t;
	return *this;
  }
  Vector3 &operator/=(T t) {
	this->x /= t;
	this->y /= t;
	this->z /= t;
	return *this;
  }

  T Dot(const Vector3 &rhs) const {
	return this->x * rhs.x + this->y * rhs.y + this->z * rhs.z;
  }
  Vector3 Cross(const Vector3 &rhs) const {
	T a = this->y * rhs.z - rhs.y * this->z;
	T b = this->z * rhs.x - rhs.z * this->x;
	T c = this->x * rhs.y - rhs.x * this->y;
	return Vector3(a, b, c);
  }

  float Norm() const { return std::sqrt(x * x + y * y + z * z); }
  Vector3<double> Normalize() const {
	float norm = this->Norm();
	return Vector3<double>(x / norm, y / norm, z / norm);
  }

  Vector3 operator-() { return Vector3(-x, -y, -z); }

 public:
  T x, y, z;
};

template <typename T>
inline Vector3<T> operator*(T t, const Vector3<T> &rhs) {
  return rhs * t;
}

using Vector3d = Vector3<double>;
using Vector3f = Vector3<float>;
using Vector3i = Vector3<int>;

template <typename T>
class Vector4 {
 public:
  Vector4() : x(0), y(0), z(0), w(1) {}
  Vector4(T x_, T y_, T z_, T w_ = 1) : x(x_), y(y_), z(z_), w(w_) {}
  Vector4(const Vector4 &rhs) : x(rhs.x), y(rhs.y), z(rhs.z), w(rhs.w) {}
  Vector4(const Vector3<T> &rhs) : x(rhs.x), y(rhs.y), z(rhs.z), w(1.0) {}
  ~Vector4() = default;

  Vector4 &operator=(const Vector4 &rhs) {
	this->x = rhs.x;
	this->y = rhs.y;
	this->z = rhs.z;
	this->w = rhs.w;
	return *this;
  }
  Vector4 &operator=(const Vector3<T> &rhs) {
	this->x = rhs.x;
	this->y = rhs.y;
	this->z = rhs.z;
	this->w = 1;
	return *this;
  }

  Vector4 operator+(const Vector4 &rhs) const {
	T a = this->x + rhs.x;
	T b = this->y + rhs.y;
	T c = this->z + rhs.z;
	T w = this->w + rhs.w;
	return Vector4(a, b, c, w);
  }
  Vector4 operator-(const Vector4 &rhs) const {
	T a = this->x - rhs.x;
	T b = this->y - rhs.y;
	T c = this->z - rhs.z;
	T w = this->w - rhs.w;
	return Vector4(a, b, c, w);
  }
  Vector4 &operator+=(const Vector4 &rhs) {
	this->x += rhs.x;
	this->y += rhs.y;
	this->z += rhs.z;
	this->w += rhs.w;
	return *this;
  }
  Vector4 &operator-=(const Vector4 &rhs) {
	this->x -= rhs.x;
	this->y -= rhs.y;
	this->z -= rhs.z;
	this->w -= rhs.w;
	return *this;
  }

  Vector4 operator*(T t) const {
	T a = this->x * t;
	T b = this->y * t;
	T c = this->z * t;
	T w = this->w * t;
	return Vector4(a, b, c, w);
  }
  Vector4 operator*(const Vector4 &rhs) {
    T a = x * rhs.x;
    T b = y * rhs.y;
    T c = z * rhs.z;
    T d = w * rhs.w;
    return Vector4(a, b, c, d);
  }
  Vector4 operator/(T t) const {
	T a = this->x / t;
	T b = this->y / t;
	T c = this->z / t;
	T w = this->w / t;
	return Vector4(a, b, c, w);
  }
  Vector4 &operator*=(T t) {
	this->x *= t;
	this->y *= t;
	this->z *= t;
	this->w *= t;
	return *this;
  }
  Vector4 &operator/=(T t) {
	this->x /= t;
	this->y /= t;
	this->z /= t;
	this->w /= t;
	return *this;
  }

  T Dot(const Vector4 &rhs) const {
	return this->x * rhs.x + this->y * rhs.y + this->z * rhs.z + this->w * rhs.w;
  }

  double Norm() const { return std::sqrt(x * x + y * y + z * z + w * w); }
  Vector4<double> Normalize() const {
	float norm = this->Norm();
	return Vector4<double>(x / norm, y / norm, z / norm, w / norm);
  }

  Vector4 operator-() const { return Vector4(-x, -y, -z, -w); }

 public:
  T x, y, z, w;
};

template <typename T>
inline Vector4<T> operator*(T t, const Vector4<T> &rhs) {
  return rhs * t;
}

template <typename T>
inline Vector4<T> Reflect(const Vector4<T> &in, const Vector4<T> &normal) {
  // in and normal are unit vectors
  double cos_theta = in.Dot(normal);
  Vector4<T> out1 = normal * cos_theta;
  Vector4<T> out2 = -in;
  return out2 + out1 * 2;
}

using Vector4d = Vector4<double>;
using Vector4f = Vector4<float>;
using Vector4i = Vector4<int>;

template <typename T>
class Vector2 {
 public:
  Vector2() : x(0), y(0) {}
  Vector2(T x_, T y_) : x(x_), y(y_) {}
  Vector2(const Vector2 &rhs) : x(rhs.x), y(rhs.y) {}
  ~Vector2() = default;

  Vector2 operator+(const Vector2 &rhs) const {
	T a = this->x + rhs.x;
	T b = this->y + rhs.y;
	return Vector2(a, b);
  }
  Vector2 operator-(const Vector2 &rhs) const {
	T a = this->x - rhs.x;
	T b = this->y - rhs.y;
	return Vector2(a, b);
  }
  Vector2 &operator+=(const Vector2 &rhs) {
	this->x += rhs.x;
	this->y += rhs.y;
	return *this;
  }
  Vector2 &operator-=(const Vector2 &rhs) {
	this->x -= rhs.x;
	this->y -= rhs.y;
	return *this;
  }

  Vector2 operator*(T t) const {
	T a = this->x * t;
	T b = this->y * t;
	return Vector2(a, b);
  }
  Vector2 operator/(T t) {
	T a = this->x / t;
	T b = this->y / t;
	return Vector2(a, b);
  }
  Vector2 &operator*=(T t) {
	this->x *= t;
	this->y *= t;
	return *this;
  }
  Vector2 &operator/=(T t) {
	this->x /= t;
	this->y /= t;
	return *this;
  }

  T Dot(const Vector2 &rhs) { return this->x * rhs.x + this->y * rhs.y; }
  T Cross(const Vector2 &rhs) { return this->x * rhs.y - rhs.x * this->y; }

  float Norm() const { return std::sqrt(x * x + y * y); }
  Vector2<float> Normalize() const {
	float norm = this->Norm();
	return Vector2<float>(x / norm, y / norm);
  }

  Vector2 operator-() { return Vector2(-x, -y); }

 public:
  T x, y;
};

template <typename T>
inline Vector2<T> operator*(T t, const Vector2<T> &rhs) {
  return rhs * t;
}

using Vector2d = Vector2<double>;
using Vector2f = Vector2<float>;
using Vector2i = Vector2<int>;

#endif //SOFTRENDERER_INCLUDE_VECTOR_H_
