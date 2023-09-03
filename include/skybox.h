#ifndef SOFTRENDERER_INCLUDE_SKYBOX_H_
#define SOFTRENDERER_INCLUDE_SKYBOX_H_

#include <string>
#include <vector>

#include "matrix.h"
#include "texture.h"
#include "vector.h"

enum class Face { kFront, kBack, kLeft, kRight, kUp, kDown };

struct SkyBoxVertex {
  SkyBoxVertex() = default;
  ~SkyBoxVertex() = default;

  Vector4d pos;
  Vector2d tex;
};

class Skybox {
 public:
  Skybox();
  ~Skybox() = default;

  void LoadSkybox(const std::string &path);
  Vector4d Sample(const Vector2d &uv, Face face);

  Vector4d TransformPos(const Matrix4d &viewport,
						const Matrix4d &project,
						const Matrix4d &view,
						const Vector4d &pos);

  const std::vector<SkyBoxVertex> &vertices() const { return vertices_; }
  const std::vector<int> &indices() const { return indices_; }

 private:
  std::vector<Texture> skybox_;
  std::vector<SkyBoxVertex> vertices_;
  std::vector<int> indices_;
};

#endif //SOFTRENDERER_INCLUDE_SKYBOX_H_
