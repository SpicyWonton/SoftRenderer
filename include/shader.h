#ifndef SOFTRENDERER_INCLUDE_SHADER_H_
#define SOFTRENDERER_INCLUDE_SHADER_H_

#include "aabb.h"
#include "light.h"

class Shader {
 public:
  Shader() = default;
  virtual ~Shader() = default;

  virtual VertexOut VertexShader(const VertexIn &in);
  virtual Vector4d FragmentShader(const VertexOut &in) = 0;

  virtual void PerspectiveCorrection(VertexOut &in);

  void set_model_matrix(Matrix4d *model) { model_matrix_ = model; set_model_normal_matrix(); }
  void set_view_matrix(Matrix4d *view) { view_matrix_ = view; }
  void set_project_matrix(Matrix4d *project) { project_matrix_ = project; }
  void set_viewport_matrix(Matrix4d *viewport) { viewport_matrix_ = viewport; }
  void albedo_texture(Texture *texture) { albedo_texture_ = texture; }
  void normal_texture(Texture *texture) { normal_texture_ = texture; }
  void set_view_pos(Vector3d *view_pos) { view_pos_ = view_pos; }

  void AddLight(Light *light) { lights_.push_back(light); }

  void TBN_matrix(const VertexIn &a, const VertexIn &b, const VertexIn &c);

 protected:
  void set_model_normal_matrix();

 protected:
  Matrix4d *model_matrix_;
  Matrix4d *view_matrix_;
  Matrix4d *project_matrix_;
  Matrix4d *viewport_matrix_;
  Matrix4d model_normal_matrix_;
  Matrix4d TBN_matrix_;
  Texture *albedo_texture_, *normal_texture_;
  Vector3d *view_pos_;
  std::vector<Light*> lights_;
};

class PhongShader : public Shader {
 public:
  PhongShader() = default;
  virtual ~PhongShader() = default;

  virtual Vector4d FragmentShader(const VertexOut &in) override;
};

class LineShader : public Shader {
 public:
  LineShader() = default;
  virtual ~LineShader() = default;

  virtual Vector4d FragmentShader(const VertexOut &in) override;
};

class PBRShader : public Shader {
 public:
  PBRShader() = default;
  virtual ~PBRShader() = default;

  virtual Vector4d FragmentShader(const VertexOut &in) override;
};

#endif //SOFTRENDERER_INCLUDE_SHADER_H_
