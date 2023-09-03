#include "frame_buffer.h"

FrameBuffer::FrameBuffer(int width, int height)
	: width_(width), height_(height), capacity_(4 * width * height) {
  color_buffer_.resize(capacity_);
  depth_buffer_.resize(width * height);
}

void FrameBuffer::ClearBuffer(const Vector4d &color) {
  for (int i = 0; i < capacity_; i += 4) {
	color_buffer_[i] = static_cast<unsigned char>(255 * color.x);
	color_buffer_[i + 1] = static_cast<unsigned char>(255 * color.y);
	color_buffer_[i + 2] = static_cast<unsigned char>(255 * color.z);
	color_buffer_[i + 3] = static_cast<unsigned char>(255 * color.w);
  }
  for (int y = 0; y < height_; y++) {
	for (int x = 0; x < width_; x++) {
	  depth_buffer_[y * width_ + x] = 1.0;
	}
  }
}

void FrameBuffer::DrawPixel(int x, int y, const Vector4d &color) {
  // points outside the frustum will be discarded
  if (x < 0 || x >= width_ || y < 0 || y >= height_)
	return;
  int index = y * 4 * width_ + 4 * x;
  color_buffer_[index] = static_cast<unsigned char>(color.x);
  color_buffer_[index + 1] = static_cast<unsigned char>(color.y);
  color_buffer_[index + 2] = static_cast<unsigned char>(color.z);
  color_buffer_[index + 3] = static_cast<unsigned char>(color.w);
}

double FrameBuffer::GetDepth(int x, int y) {
  if (x < 0 || x >= width_ || y < 0 || y >= height_)
	return 1.0;
  return depth_buffer_[y * width_ + x];
}

void FrameBuffer::SetDepth(int x, int y, double depth) {
  if (x < 0 || x >= width_ || y < 0 || y >= height_)
	return;
  depth_buffer_[y * width_ + x] = depth;
}

ShadowBuffer::ShadowBuffer(int width, int height)
	: width_(width), height_(height), capacity_(width * height) {
  shadow_buffer_.resize(capacity_);
  shadow_texture_.resize(3 * capacity_);
}

void ShadowBuffer::ClearBuffer() {
  for (int y = 0; y < height_; y++) {
	for (int x = 0; x < width_; x++) {
	  shadow_buffer_[y * width_ + x] = 0.0;
	}
  }
}

double ShadowBuffer::GetDepth(int x, int y) {
  if (x < 0 || x >= width_ || y < 0 || y >= height_)
	return 0.0;
  return shadow_buffer_[y * width_ + x];
}

void ShadowBuffer::SetDepth(int x, int y, double depth) {
  if (x < 0 || x >= width_ || y < 0 || y >= height_)
	return;
  shadow_buffer_[y * width_ + x] = depth;
}