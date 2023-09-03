#ifndef SOFTRENDERER_INCLUDE_SHADOW_MAP_H_
#define SOFTRENDERER_INCLUDE_SHADOW_MAP_H_

#include "aabb.h"
#include "light.h"
#include "mesh.h"

class ShadowMap {
 public:
  ShadowMap() = default;
  ~ShadowMap() = default;

  void AddLight(Light *light) { lights_.push_back(light); }
  void AddMesh(Mesh *mesh) { meshes_.push_back(mesh); }

  void RenderShadowMap(const Matrix4d &viewport_matrix);

 private:
  void SetLights();
  void SetShadowTexture(const VertexOut &p1,
						const VertexOut &p2,
						const VertexOut &p3,
						Light *light);
  VertexOut TransformVertex(const VertexIn &in, const Matrix4d &model_matrix, Light *light);

 private:
  std::vector<Light*> lights_;
  std::vector<Mesh*> meshes_;
  char name_[17];
};

#endif //SOFTRENDERER_INCLUDE_SHADOW_MAP_H_
