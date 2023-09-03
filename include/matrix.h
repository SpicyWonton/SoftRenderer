#ifndef SOFTRENDERER_INCLUDE_MATRIX_H_
#define SOFTRENDERER_INCLUDE_MATRIX_H_

#include <iostream>

#include "aabb.h"
#include "math_util.h"
#include "vector.h"

template<typename T>
class Matrix4 {
 public:
  Matrix4() = default;
  Matrix4(T a00, T a01, T a02, T a03, T a10, T a11, T a12, T a13, T a20, T a21,
		  T a22, T a23, T a30, T a31, T a32, T a33)
	  : data{a00, a01, a02, a03, a10, a11, a12, a13,
			 a20, a21, a22, a23, a30, a31, a32, a33} {}
  ~Matrix4() = default;

  Matrix4 &operator=(const Matrix4 &rhs) {
	for (int i = 0; i < 4; i++) {
	  for (int j = 0; j < 4; j++) {
		data[i][j] = rhs.data[i][j];
	  }
	}
	return *this;
  }

  Matrix4 operator+(const Matrix4 &rhs) {
	return Matrix4(
		data[0][0] + rhs(0, 0), data[0][1] + rhs(0, 1), data[0][2] + rhs(0, 2),
		data[0][3] + rhs(0, 3), data[1][0] + rhs(1, 0), data[1][1] + rhs(1, 1),
		data[1][2] + rhs(1, 2), data[1][3] + rhs(1, 3), data[2][0] + rhs(2, 0),
		data[2][1] + rhs(2, 1), data[2][2] + rhs(2, 2), data[2][3] + rhs(2, 3),
		data[3][0] + rhs(3, 0), data[3][1] + rhs(3, 1), data[3][2] + rhs(3, 2),
		data[3][3] + rhs(3, 3));
  }
  Matrix4 operator-(const Matrix4 &rhs) {
	return Matrix4(
		data[0][0] - rhs(0, 0), data[0][1] - rhs(0, 1), data[0][2] - rhs(0, 2),
		data[0][3] - rhs(0, 3), data[1][0] - rhs(1, 0), data[1][1] - rhs(1, 1),
		data[1][2] - rhs(1, 2), data[1][3] - rhs(1, 3), data[2][0] - rhs(2, 0),
		data[2][1] - rhs(2, 1), data[2][2] - rhs(2, 2), data[2][3] - rhs(2, 3),
		data[3][0] - rhs(3, 0), data[3][1] - rhs(3, 1), data[3][2] - rhs(3, 2),
		data[3][3] - rhs(3, 3));
  }
  Matrix4 &operator+=(const Matrix4 &rhs) {
	data[0][0] += rhs(0, 0);
	data[0][1] += rhs(0, 1);
	data[0][2] += rhs(0, 2);
	data[0][3] += rhs(0, 3);
	data[1][0] += rhs(1, 0);
	data[1][1] += rhs(1, 1);
	data[1][2] += rhs(1, 2);
	data[1][3] += rhs(1, 3);
	data[2][0] += rhs(2, 0);
	data[2][1] += rhs(2, 1);
	data[2][2] += rhs(2, 2);
	data[2][3] += rhs(2, 3);
	data[3][0] += rhs(3, 0);
	data[3][1] += rhs(3, 1);
	data[3][2] += rhs(3, 2);
	data[3][3] += rhs(3, 3);
	return *this;
  }
  Matrix4 &operator-=(const Matrix4 &rhs) {
	data[0][0] -= rhs(0, 0);
	data[0][1] -= rhs(0, 1);
	data[0][2] -= rhs(0, 2);
	data[0][3] -= rhs(0, 3);
	data[1][0] -= rhs(1, 0);
	data[1][1] -= rhs(1, 1);
	data[1][2] -= rhs(1, 2);
	data[1][3] -= rhs(1, 3);
	data[2][0] -= rhs(2, 0);
	data[2][1] -= rhs(2, 1);
	data[2][2] -= rhs(2, 2);
	data[2][3] -= rhs(2, 3);
	data[3][0] -= rhs(3, 0);
	data[3][1] -= rhs(3, 1);
	data[3][2] -= rhs(3, 2);
	data[3][3] -= rhs(3, 3);
	return *this;
  }

  Matrix4 operator*(const Matrix4 &rhs) {
	T a00 = data[0][0] * rhs(0, 0) + data[0][1] * rhs(1, 0) +
		data[0][2] * rhs(2, 0) + data[0][3] * rhs(3, 0);
	T a01 = data[0][0] * rhs(0, 1) + data[0][1] * rhs(1, 1) +
		data[0][2] * rhs(2, 1) + data[0][3] * rhs(3, 1);
	T a02 = data[0][0] * rhs(0, 2) + data[0][1] * rhs(1, 2) +
		data[0][2] * rhs(2, 2) + data[0][3] * rhs(3, 2);
	T a03 = data[0][0] * rhs(0, 3) + data[0][1] * rhs(1, 3) +
		data[0][2] * rhs(2, 3) + data[0][3] * rhs(3, 3);

	T a10 = data[1][0] * rhs(0, 0) + data[1][1] * rhs(1, 0) +
		data[1][2] * rhs(2, 0) + data[1][3] * rhs(3, 0);
	T a11 = data[1][0] * rhs(0, 1) + data[1][1] * rhs(1, 1) +
		data[1][2] * rhs(2, 1) + data[1][3] * rhs(3, 1);
	T a12 = data[1][0] * rhs(0, 2) + data[1][1] * rhs(1, 2) +
		data[1][2] * rhs(2, 2) + data[1][3] * rhs(3, 2);
	T a13 = data[1][0] * rhs(0, 3) + data[1][1] * rhs(1, 3) +
		data[1][2] * rhs(2, 3) + data[1][3] * rhs(3, 3);

	T a20 = data[2][0] * rhs(0, 0) + data[2][1] * rhs(1, 0) +
		data[2][2] * rhs(2, 0) + data[2][3] * rhs(3, 0);
	T a21 = data[2][0] * rhs(0, 1) + data[2][1] * rhs(1, 1) +
		data[2][2] * rhs(2, 1) + data[2][3] * rhs(3, 1);
	T a22 = data[2][0] * rhs(0, 2) + data[2][1] * rhs(1, 2) +
		data[2][2] * rhs(2, 2) + data[2][3] * rhs(3, 2);
	T a23 = data[2][0] * rhs(0, 3) + data[2][1] * rhs(1, 3) +
		data[2][2] * rhs(2, 3) + data[2][3] * rhs(3, 3);

	T a30 = data[3][0] * rhs(0, 0) + data[3][1] * rhs(1, 0) +
		data[3][2] * rhs(2, 0) + data[3][3] * rhs(3, 0);
	T a31 = data[3][0] * rhs(0, 1) + data[3][1] * rhs(1, 1) +
		data[3][2] * rhs(2, 1) + data[3][3] * rhs(3, 1);
	T a32 = data[3][0] * rhs(0, 2) + data[3][1] * rhs(1, 2) +
		data[3][2] * rhs(2, 2) + data[3][3] * rhs(3, 2);
	T a33 = data[3][0] * rhs(0, 3) + data[3][1] * rhs(1, 3) +
		data[3][2] * rhs(2, 3) + data[3][3] * rhs(3, 3);

	return Matrix4(a00, a01, a02, a03, a10, a11, a12, a13, a20, a21, a22, a23,
				   a30, a31, a32, a33);
  }
  Vector4<T> operator*(const Vector4<T> &rhs) const {
	T x = data[0][0] * rhs.x + data[0][1] * rhs.y + data[0][2] * rhs.z +
		data[0][3] * rhs.w;
	T y = data[1][0] * rhs.x + data[1][1] * rhs.y + data[1][2] * rhs.z +
		data[1][3] * rhs.w;
	T z = data[2][0] * rhs.x + data[2][1] * rhs.y + data[2][2] * rhs.z +
		data[2][3] * rhs.w;
	T w = data[3][0] * rhs.x + data[3][1] * rhs.y + data[3][2] * rhs.z +
		data[3][3] * rhs.w;
	return Vector4<T>(x, y, z, w);
  }

  T operator()(int x, int y) const { return data[x][y]; }

  void SetZero() {
	for (int i = 0; i < 4; i++) {
	  for (int j = 0; j < 4; j++) {
		data[i][j] = 0;
	  }
	}
  }
  void SetIdentity() {
	for (int i = 0; i < 4; i++) {
	  for (int j = 0; j < 4; j++) {
		if (i == j)
		  data[i][j] = 1;
		else
		  data[i][j] = 0;
	  }
	}
  }
  void SetViewport(int left, int top, int width, int height) {
	SetIdentity();
	data[0][0] = static_cast<T>(width) / 2.0f;
	data[0][3] = left + data[0][0];
	data[1][1] = -static_cast<T>(height) / 2.0f;
	data[1][3] = top - data[1][1];
  }
  void SetTranslation(const Vector3<T> &v) {
	SetIdentity();
	data[0][3] = v.x;
	data[1][3] = v.y;
	data[2][3] = v.z;
  }
  void SetScale(const Vector3<T> &v) {
	SetIdentity();
	data[0][0] = v.x;
	data[1][1] = v.y;
	data[2][2] = v.z;
  }
  void SetRotationX(double angle) {
	SetIdentity();
	data[1][1] = cos(kPI * angle / 180.0);
	data[1][2] = -sin(kPI * angle / 180.0);
	data[2][1] = -data[1][2];
	data[2][2] = data[1][1];
  }
  void SetRotationY(double angle) {
	SetIdentity();
	data[0][0] = cos(kPI * angle / 180.0);
	data[0][2] = sin(kPI * angle / 180.0);
	data[2][0] = -data[0][2];
	data[2][2] = data[0][0];
  }
  void SetRotationZ(double angle) {
	SetIdentity();
	data[0][0] = cos(kPI * angle / 180.0);
	data[0][1] = -sin(kPI * angle / 180.0);
	data[1][0] = -data[0][1];
	data[1][1] = data[0][0];
  }
  void SetRotationAxis(double angle, const Vector3d &axis) {
	Vector3d u = axis.Normalize();

	double cos_theta = cos(kPI * angle / 180.0);
	double sin_theta = sin(kPI * angle / 180.0);

	SetIdentity();

	data[0][0] = u.x * u.x * (1.0 - cos_theta) + cos_theta;
	data[0][1] = u.x * u.y * (1.0 - cos_theta) - u.z * sin_theta;
	data[0][2] = u.x * u.z * (1.0 - cos_theta) + u.y * sin_theta;
	data[1][0] = u.x * u.y * (1.0 - cos_theta) + u.z * sin_theta;
	data[1][1] = u.y * u.y * (1.0 - cos_theta) + cos_theta;
	data[1][2] = u.y * u.z * (1.0 - cos_theta) - u.x * sin_theta;
	data[2][0] = u.x * u.z * (1.0 - cos_theta) - u.y * sin_theta;
	data[2][1] = u.y * u.z * (1.0 - cos_theta) + u.x * sin_theta;
	data[2][2] = u.z * u.z * (1.0 - cos_theta) + cos_theta;
  }
  void SetView(const Vector3d &eye, const Vector3d &z_axis, const Vector3d &up) {
	// dir and up are unit vectors
	SetIdentity();

	// Vector3d z_axis = eye - target;
	Vector3d x_axis = up.Cross(z_axis);
	x_axis = x_axis.Normalize();
	Vector3d y_axis = z_axis.Cross(x_axis);
	y_axis = y_axis.Normalize();

	data[0][0] = x_axis.x;
	data[0][1] = x_axis.y;
	data[0][2] = x_axis.z;

	data[1][0] = y_axis.x;
	data[1][1] = y_axis.y;
	data[1][2] = y_axis.z;

	data[2][0] = z_axis.x;
	data[2][1] = z_axis.y;
	data[2][2] = z_axis.z;

	data[0][3] = -x_axis.Dot(eye);
	data[1][3] = -y_axis.Dot(eye);
	data[2][3] = -z_axis.Dot(eye);
  }
  void SetOrtho(double right, double top, double near, double far) {
	SetIdentity();
	data[0][0] = 1.0 / right;
	data[1][1] = 1.0 / top;
	data[2][2] = 2.0 / (near - far);
	data[2][3] = (near + far) / (far - near);
  }
  void SetPerspective(double fovy, double aspect, double near, double far) {
	// near, far > 0
	SetZero();
	double tan_half_fovy = tan(kPI * fovy / 360.0);
	data[0][0] = 1.0 / (tan_half_fovy * aspect);
	data[1][1] = 1.0 / tan_half_fovy;
	data[2][2] = (near + far) / (near - far);
	data[2][3] = (2.0 * far * near) / (near - far);
	data[3][2] = -1;
  }
  Matrix4 Inverse() const {
	double det;
	Matrix4 adjoint_matrix = AdjointMatrix();

	det = data[0][0] * adjoint_matrix(0, 0) + data[0][1] * adjoint_matrix(1, 0)
		+ data[0][2] * adjoint_matrix(2, 0) + data[0][3] * adjoint_matrix(3, 0);

	if (det == 0.0) {
	  std::cout << "det can not be zero!" << std::endl;
	  exit(1);
	}

	det = 1.0 / det;
	for (int y = 0; y < 4; y++) {
	  for (int x = 0; x < 4; x++) {
	    adjoint_matrix.data[x][y] *= det;
	  }
	}

	return adjoint_matrix;
  }
  Matrix4 InverseHomo() const {
    Matrix4 m1(data[0][0], data[0][1], data[0][2], 0.0,
			   data[1][0], data[1][1], data[1][2], 0.0,
			   data[2][0], data[2][1], data[2][2], 0.0,
			   data[3][0], data[3][1], data[3][2], data[3][3]);
    Matrix4 m2(1.0, 0.0, 0.0, -data[0][3],
			   0.0, 1.0, 0.0, -data[1][3],
			   0.0, 0.0, 1.0, -data[2][3],
			   0.0, 0.0, 0.0, 1.0);
    return m1.Inverse() * m2;
  }
  Matrix4 Transpose() const {
    Matrix4 t_matrix;
    for (int y = 0; y < 4; y++) {
      for (int x = 0; x < 4; x++) {
        t_matrix.data[x][y] = data[y][x];
      }
    }
    return t_matrix;
  }
  Matrix4 AdjointMatrix() const {
	Matrix4 adjoint_matrix;
	//A11
	adjoint_matrix.data[0][0] = data[1][1] * (data[2][2] * data[3][3] - data[3][2] * data[2][3])
		- data[1][2] * (data[2][1] * data[3][3] - data[3][1] * data[2][3])
		+ data[1][3] * (data[2][1] * data[3][2] - data[3][1] * data[2][2]);
	//A12
	adjoint_matrix.data[1][0] = -(data[1][0] * (data[2][2] * data[3][3] - data[3][2] * data[2][3])
		- data[1][2] * (data[2][0] * data[3][3] - data[3][0] * data[2][3])
		+ data[1][3] * (data[2][0] * data[3][2] - data[3][0] * data[2][2]));
	//A13
	adjoint_matrix.data[2][0] = data[1][0] * (data[2][1] * data[3][3] - data[3][1] * data[2][3])
		- data[1][1] * (data[2][0] * data[3][3] - data[3][0] * data[2][3])
		+ data[1][3] * (data[2][0] * data[3][1] - data[3][0] * data[2][1]);
	//A14
	adjoint_matrix.data[3][0] = -(data[1][0] * (data[2][1] * data[3][2] - data[3][1] * data[2][2])
		- data[1][1] * (data[2][0] * data[3][2] - data[3][0] * data[2][2])
		+ data[1][2] * (data[2][0] * data[3][1] - data[3][0] * data[2][1]));
	//A21
	adjoint_matrix.data[0][1] = -(data[0][1] * (data[2][2] * data[3][3] - data[3][2] * data[2][3])
		- data[0][2] * (data[2][1] * data[3][3] - data[3][1] * data[2][3])
		+ data[0][3] * (data[2][1] * data[3][2] - data[3][1] * data[2][2]));
	//A22
	adjoint_matrix.data[1][1] = data[0][0] * (data[2][2] * data[3][3] - data[3][2] * data[2][3])
		- data[0][2] * (data[2][0] * data[3][3] - data[3][0] * data[2][3])
		+ data[0][3] * (data[2][0] * data[3][2] - data[3][0] * data[2][2]);
	//A23
	adjoint_matrix.data[2][1] = -(data[0][0] * (data[2][1] * data[3][3] - data[3][1] * data[2][3])
		- data[0][1] * (data[2][0] * data[3][3] - data[3][0] * data[2][3])
		+ data[0][3] * (data[2][0] * data[3][1] - data[3][0] * data[2][1]));
	//A24
	adjoint_matrix.data[3][1] = data[0][0] * (data[2][1] * data[3][2] - data[3][1] * data[2][2])
		- data[0][1] * (data[2][0] * data[3][2] - data[3][0] * data[2][2])
		+ data[0][2] * (data[2][0] * data[3][1] - data[3][0] * data[2][1]);
	//A31
	adjoint_matrix.data[0][2] = data[0][1] * (data[1][2] * data[3][3] - data[3][2] * data[1][3])
		- data[0][2] * (data[1][1] * data[3][3] - data[3][1] * data[1][3])
		+ data[0][3] * (data[1][1] * data[3][2] - data[3][1] * data[1][2]);
	//A32
	adjoint_matrix.data[1][2] = -(data[0][0] * (data[1][2] * data[3][3] - data[3][2] * data[1][3])
		- data[0][2] * (data[1][0] * data[3][3] - data[3][0] * data[1][3])
		+ data[0][3] * (data[1][0] * data[3][2] - data[3][0] * data[1][2]));
	//A33
	adjoint_matrix.data[2][2] = data[0][0] * (data[1][1] * data[3][3] - data[3][1] * data[1][3])
		- data[0][1] * (data[1][0] * data[3][3] - data[3][0] * data[1][3])
		+ data[0][3] * (data[1][0] * data[3][1] - data[3][0] * data[1][1]);
	//A34
	adjoint_matrix.data[3][2] = -(data[0][0] * (data[1][1] * data[3][2] - data[3][1] * data[1][2])
		- data[0][1] * (data[1][0] * data[3][2] - data[3][0] * data[1][2])
		+ data[0][2] * (data[1][0] * data[3][1] - data[3][0] * data[1][1]));
	//A41
	adjoint_matrix.data[0][3] = -(data[0][1] * (data[1][2] * data[2][3] - data[2][2] * data[1][3])
		- data[0][2] * (data[1][1] * data[2][3] - data[2][1] * data[1][3])
		+ data[0][3] * (data[1][1] * data[2][2] - data[2][1] * data[1][2]));
	//A42
	adjoint_matrix.data[1][3] = data[0][0] * (data[1][2] * data[2][3] - data[2][2] * data[1][3])
		- data[0][2] * (data[1][0] * data[2][3] - data[2][0] * data[1][3])
		+ data[0][3] * (data[1][0] * data[2][2] - data[2][0] * data[1][2]);
	//A43
	adjoint_matrix.data[2][3] = -(data[0][0] * (data[1][1] * data[2][3] - data[2][1] * data[1][3])
		- data[0][1] * (data[1][0] * data[2][3] - data[2][0] * data[1][3])
		+ data[0][3] * (data[1][0] * data[2][1] - data[2][0] * data[1][1]));
	//A44
	adjoint_matrix.data[3][3] = data[0][0] * (data[1][1] * data[2][2] - data[2][1] * data[1][2])
		- data[0][1] * (data[1][0] * data[2][2] - data[2][0] * data[1][2])
		+ data[0][2] * (data[1][0] * data[2][1] - data[2][0] * data[1][1]);

	return adjoint_matrix;
  }
  Matrix4 AdjointMatrix33() const {
	Matrix4 adjoint_matrix;
	//A11
	adjoint_matrix.data[0][0] = data[1][1] * data[2][2] - data[1][2] * data[2][1];
	//A12
	adjoint_matrix.data[1][0] = -(data[1][0] * data[2][2] - data[1][2] * data[2][0]);
	//A13
	adjoint_matrix.data[2][0] = data[1][0] * data[2][1] - data[1][1] * data[2][0];
	//A21
	adjoint_matrix.data[0][1] = -(data[0][1] * data[2][2] - data[0][2] * data[2][1]);
	//A22
	adjoint_matrix.data[1][1] = data[0][0] * data[2][2] - data[0][2] * data[2][0];
	//A23
	adjoint_matrix.data[2][1] = -(data[0][0] * data[2][1] - data[0][1] * data[2][0]);
	//A31
	adjoint_matrix.data[0][2] = data[0][1] * data[1][2] - data[0][2] * data[1][1];
	//A32
	adjoint_matrix.data[1][2] = -(data[0][0] * data[1][2] - data[0][2] * data[1][0]);
	//A33
	adjoint_matrix.data[2][2] = data[0][0] * data[1][1] - data[0][1] * data[1][0];

	adjoint_matrix.data[0][3] = data[0][3];
	adjoint_matrix.data[1][3] = data[1][3];
	adjoint_matrix.data[2][3] = data[2][3];
	adjoint_matrix.data[3][3] = data[3][3];
	adjoint_matrix.data[3][0] = data[3][0];
	adjoint_matrix.data[3][1] = data[3][1];
	adjoint_matrix.data[3][2] = data[3][2];

	return adjoint_matrix;
  }
  Matrix4 Transpose33() const {
    Matrix4 t_matrix;

    for (int y = 0; y < 3; y++) {
      for (int x = 0; x < 3; x++) {
        t_matrix.data[x][y] = data[y][x];
      }
    }

	t_matrix.data[0][3] = data[0][3];
	t_matrix.data[1][3] = data[1][3];
	t_matrix.data[2][3] = data[2][3];
	t_matrix.data[3][3] = data[3][3];
	t_matrix.data[3][0] = data[3][0];
	t_matrix.data[3][1] = data[3][1];
	t_matrix.data[3][2] = data[3][2];

	return t_matrix;
  }

  void SetMatrix(const Vector3d &v1, const Vector3d &v2, const Vector3d &v3) {
    data[0][0] = v1.x;
    data[1][0] = v1.y;
    data[2][0] = v1.z;
    data[3][0] = 0.0;

    data[0][1] = v2.x;
    data[1][1] = v2.y;
    data[2][1] = v2.z;
    data[3][1] = 0.0;

    data[0][2] = v3.x;
    data[1][2] = v3.y;
    data[2][2] = v3.z;
    data[3][2] = 0.0;

    data[0][3] = 0.0;
    data[1][3] = 0.0;
    data[2][3] = 0.0;
    data[3][3] = 1.0;
  }

 private:
  T data[4][4];
};

template<typename T>
inline std::ostream &operator<<(std::ostream &os, const Matrix4<T> &rhs) {
  os << rhs(0, 0) << " " << rhs(0, 1) << " " << rhs(0, 2) << " " << rhs(0, 3)
	 << std::endl;
  os << rhs(1, 0) << " " << rhs(1, 1) << " " << rhs(1, 2) << " " << rhs(1, 3)
	 << std::endl;
  os << rhs(2, 0) << " " << rhs(2, 1) << " " << rhs(2, 2) << " " << rhs(2, 3)
	 << std::endl;
  os << rhs(3, 0) << " " << rhs(3, 1) << " " << rhs(3, 2) << " " << rhs(3, 3)
	 << std::endl;
  return os;
}

using Matrix4d = Matrix4<double>;
using Matrix4f = Matrix4<float>;
using Matrix4i = Matrix4<int>;

#endif //SOFTRENDERER_INCLUDE_MATRIX_H_
