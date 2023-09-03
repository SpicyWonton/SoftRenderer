#include "camera.h"

#include "math_util.h"

Camera::Camera(const Vector3d &eye, const Vector3d &dir, const Vector3d &up,
			   double speed)
	: eye_(eye),
	  dir_(dir.Normalize()),
	  up_(up.Normalize()),
	  pitch_(0.0),
	  yaw_(90.0),
	  speed_(speed) {
  view_matrix_.SetView(eye_, dir_, up_);
}

void Camera::MoveForward(double delta_time) {
  eye_ -= delta_time * speed_ * dir_;
}
void Camera::MoveBack(double delta_time) {
  eye_ += delta_time * speed_ * dir_;
}
void Camera::MoveLeft(double delta_time) {
  eye_ -= delta_time * speed_ * (up_.Cross(dir_)).Normalize();
}
void Camera::MoveRight(double delta_time) {
  eye_ += delta_time * speed_ * (up_.Cross(dir_)).Normalize();
}

void Camera::Rotate(int offset_x, int offset_y) {
  if (offset_x == 0 && offset_y == 0) return;
  yaw_ += offset_x * 0.25;
  pitch_ += offset_y * 0.25;
  if (pitch_ > 89.0) pitch_ = 89.0;
  if (pitch_ < -89.0) pitch_ = -89.0;
  Vector3d dir;
  dir.x = cos(Radian(pitch_)) * cos(Radian(yaw_));
  dir.y = sin(Radian(pitch_));
  dir.z = cos(Radian(pitch_)) * sin(Radian(yaw_));
  dir_ = dir.Normalize();
}
