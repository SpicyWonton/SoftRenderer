#ifndef SOFTRENDERER_INCLUDE_TEXTURE_H_
#define SOFTRENDERER_INCLUDE_TEXTURE_H_

#include "vector.h"

class Texture {
 public:
  Texture() : data_(nullptr) {}
  ~Texture();

  Vector4d Sample(const Vector2d &tex);
  bool LoadImage(const char *path);

 private:
  int width_, height_;
  int channels_;
  unsigned char *data_;
};

#endif //SOFTRENDERER_INCLUDE_TEXTURE_H_
