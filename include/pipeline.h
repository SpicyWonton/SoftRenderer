#ifndef SOFTRENDERER_INCLUDE_PIPELINE_H_
#define SOFTRENDERER_INCLUDE_PIPELINE_H_

#include <vector>

#include "camera.h"
#include "global_config.h"
#include "shader.h"
#include "shadow_map.h"
#include "skybox.h"
#include "frame_buffer.h"
#include "matrix.h"
#include "mesh.h"
#include "texture.h"
#include "vertex.h"

enum class ClipPlane {
  kWZero,
  kNear, kFar,
  kLeft, kRight,
  kTop, kBottom
};

class Pipeline {
 public:
  Pipeline(int width, int height);
  ~Pipeline();

  void ClearBuffer(const Vector4d &color);
  void SwapBuffer();

  void SwitchMode(RenderMode mode);

  void RenderShadowMap();
  void Draw(RenderMode mode);

  void AddLight(Light *light) {
	shader_->AddLight(light);
	shadow_map_->AddLight(light);
  }
  void AddMesh(Mesh *mesh) {
	meshes_.push_back(mesh);
	shadow_map_->AddMesh(mesh);
  }
  void SetCamera(Camera *c) {
	shader_->set_view_matrix(c->view_matrix());
	shader_->set_view_pos(c->eye());
	view_matrix_ = c->view_matrix();
  }
  void SetSkybox(Skybox *skybox) {
	skybox_ = skybox;
  }
  void SetProjectMatrix(Matrix4d *p) {
	shader_->set_project_matrix(p);
	project_matrix_ = p;
  }

  static bool InTriangle(const Vector2d &p, const Vector3d &a, const Vector3d &b, const Vector3d &c,
						 double &alpha, double &beta, double &gamma);

  unsigned char *ColorBuffer() { return front_buffer_->color_buffer(); }
  Shader *shader() { return shader_; }

 private:
  bool BackFaceCulling(const Vector4d &v1, const Vector4d &v2, const Vector4d &v3);
  std::vector<VertexOut> HomogeneousClipping(const VertexOut &p1,
											 const VertexOut &p2,
											 const VertexOut &p3);
  std::vector<VertexOut> ClipWithPlane(ClipPlane type,
									   int &num_vertex,
									   const std::vector<VertexOut> &vertices);
  void PerspectiveDivision(VertexOut &v);
  void DrawLine(const VertexOut &p1, const VertexOut &p2);
  void DrawTriangle(const VertexOut &p1, const VertexOut &p2, const VertexOut &p3);
  void DrawSkybox(RenderMode mode);
  void DrawSkyboxTriangle(const SkyBoxVertex &v1,
						  const SkyBoxVertex &v2,
						  const SkyBoxVertex &v3,
						  int index);

 private:
  int width_, height_;
  Shader *shader_;
  ShadowMap *shadow_map_;
  FrameBuffer *front_buffer_, *back_buffer_;
  Matrix4d viewport_matrix_, *view_matrix_, *project_matrix_;
  std::vector<Mesh *> meshes_;
  Skybox *skybox_;
};

#endif //SOFTRENDERER_INCLUDE_PIPELINE_H_
