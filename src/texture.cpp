#include "texture.h"

#include <cstdio>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Texture::~Texture() {
  if (data_) delete data_;
}

// use texture coordinate to get texture color
Vector4d Texture::Sample(const Vector2d &tex) {
  if (!data_) return Vector4d{};
  // u and v range from 0 to 1
  double u = tex.x - floor(tex.x);
  double v = tex.y - floor(tex.y);
  int x = static_cast<int>(u * (width_ - 1));
  int y = static_cast<int>(v * (height_ - 1));
  int index = (y * width_ + x) * channels_;
  return Vector4d(*(data_ + index), *(data_ + index + 1), *(data_ + index + 2), 0.0);
}

bool Texture::LoadImage(const char *path) {
  if (data_) delete data_;
  data_ = nullptr;
  data_ = stbi_load(path, &width_, &height_, &channels_, 0);
  if (!data_) printf("Failed to load texture image!\n");
  return data_ != nullptr;
}