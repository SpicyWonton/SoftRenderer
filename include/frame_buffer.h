#ifndef SOFTRENDERER_INCLUDE_FRAME_BUFFER_H_
#define SOFTRENDERER_INCLUDE_FRAME_BUFFER_H_

#include <iostream>
#include <vector>

#include "vector.h"

class FrameBuffer {
 public:
  FrameBuffer(int width, int height);
  ~FrameBuffer() = default;

  void ClearBuffer(const Vector4d &color);
  void DrawPixel(int x, int y, const Vector4d &color);
  double GetDepth(int x, int y);
  void SetDepth(int x, int y, double depth);

  int width() const { return width_; }
  int height() const { return height_; }
  unsigned char *color_buffer() { return color_buffer_.data(); }

 private:
  int width_, height_, capacity_;
  std::vector<unsigned char> color_buffer_;
  std::vector<double> depth_buffer_;
};

class ShadowBuffer {
 public:
  ShadowBuffer(int width, int height);
  ~ShadowBuffer() = default;

  void ClearBuffer();
  double GetDepth(int x, int y);
  void SetDepth(int x, int y, double depth);

  int width() const { return width_; }
  int height() const {return height_; }
  unsigned char *shadow_texture() {
    for (int i = 0; i < capacity_; i++) {
      int j = 3 * i;
	  shadow_texture_[j] = static_cast<unsigned char>(shadow_buffer_[i] * 255.0);
	  shadow_texture_[j + 1] = static_cast<unsigned char>(shadow_buffer_[i] * 255.0);
	  shadow_texture_[j + 2] = static_cast<unsigned char>(shadow_buffer_[i] * 255.0);
    }
    return shadow_texture_.data();
  }

 private:
  int width_, height_, capacity_;
  std::vector<double> shadow_buffer_;
  std::vector<unsigned char> shadow_texture_;
};

#endif //SOFTRENDERER_INCLUDE_FRAME_BUFFER_H_
