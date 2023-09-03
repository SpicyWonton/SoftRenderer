#ifndef SOFTRENDERER_INCLUDE_CAMERA_H_
#define SOFTRENDERER_INCLUDE_CAMERA_H_

#include "matrix.h"

class Camera {
 public:
  Camera() = default;
  Camera(const Vector3d &eye, const Vector3d &dir, const Vector3d &up,
		 double speed = 0.01);
  ~Camera() = default;

  void MoveForward(double delta_time);
  void MoveBack(double delta_time);
  void MoveLeft(double delta_time);
  void MoveRight(double delta_time);
  void Rotate(int offset_x, int offset_y);

  Vector3d* eye() { return &eye_; }
  Matrix4d* view_matrix() { return &view_matrix_; }

  void UpdateView() { view_matrix_.SetView(eye_, dir_, up_); }

 private:
  Vector3d eye_;
  Vector3d dir_;
  Vector3d up_;
  Matrix4d view_matrix_;
  double pitch_, yaw_;
  double speed_;
};

#endif //SOFTRENDERER_INCLUDE_CAMERA_H_
