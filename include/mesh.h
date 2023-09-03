#ifndef SOFTRENDERER_INCLUDE_MESH_H_
#define SOFTRENDERER_INCLUDE_MESH_H_

#include <string>
#include <vector>

#include "aabb.h"
#include "matrix.h"
#include "texture.h"
#include "vertex.h"

class Mesh {
 public:
  Mesh() { model_matrix.SetIdentity(); }
  ~Mesh() = default;

  void LoadObjFile(const std::string &path);
  void LoadAlbedoTexture(const std::string &path);
  void LoadNormalTexture(const std::string &path);
  void LoadAABB();

  void set_model_matrix(const Matrix4d &model) { model_matrix = model; }

  AABB aabb() { return aabb_; }
  void aabb(const AABB &aabb) { aabb_ = aabb; }

 private:
  std::vector<std::string> Split(const std::string &str, const std::string &delimiter = "/");

 public:
  std::vector<VertexIn> vertices;
  std::vector<int> indices;
  Texture albedo_texture, normal_texture;
  Matrix4d model_matrix;

 private:
  AABB aabb_;
};

#endif //SOFTRENDERER_INCLUDE_MESH_H_
