#ifndef SOFTRENDERER_INCLUDE_LIGHT_H_
#define SOFTRENDERER_INCLUDE_LIGHT_H_

#include "frame_buffer.h"
#include "matrix.h"
#include "mesh.h"
#include "vector.h"

enum class LightType {
  kDir, kPoint, kSpot
};

class Light {
 public:
  Light(const Vector4d &light_pos = Vector4d{},
		const Vector4d &light_color = Vector4d(2.0, 2.0, 2.0),
		const Vector4d &ambient = Vector4d(0.2, 0.2, 0.2),
		const Vector4d &diffuse = Vector4d(0.5, 0.5, 0.5),
		const Vector4d &specular = Vector4d(1.0, 1.0, 1.0)) :
	  light_pos_(light_pos),
	  light_color_(light_color),
	  ambient_(ambient),
	  diffuse_(diffuse),
	  specular_(specular) {}
  virtual ~Light() { if (shadow_buffer_) delete shadow_buffer_; }

  // all the parameters are in world coordinates
  virtual Vector4d Lighting(const Vector4d &normal,
							const Vector4d &pos,
							const Vector4d &view_pos,
							const Vector4d &albedo,
							bool occlude) = 0;
  // usedin pbr, all the parameters are in world coordinates
  virtual Vector4d PBRLighting(const Vector4d &normal,
							   const Vector4d &pos,
							   const Vector4d &view_pos,
							   const Vector4d &albedo,
							   bool occlude) = 0;

  LightType type() const { return type_; }
  Vector4d light_pos() const { return light_pos_; }
  void light_pos(const Vector4d &light_pos) { light_pos_ = light_pos; }
  Matrix4d view_matrix() { return view_matrix_; }
  void view_matrix(const Vector3d &pos,
				   const Vector3d &dir,
				   const Vector3d &up) { view_matrix_.SetView(pos, dir, up); }
  Matrix4d project_matrix() { return project_matrix_; }
  void project_matrix(const std::vector<Mesh *> &meshes) {
	AABB aabb;
	Matrix4d mat;
	for (int i = 0; i < meshes.size(); i++) {
	  mat = view_matrix_ * meshes[i]->model_matrix;
	  for (int j = 0; j < meshes[i]->vertices.size(); j++) {
		aabb = AABB::Union(aabb, mat * meshes[i]->vertices[j].local_position);
	  }
	}
	light_pos_ = Vector4d((aabb.min().x + aabb.max().x) / 2.0,
						  (aabb.min().y + aabb.max().y) / 2.0,
						  aabb.max().z,
						  1.0);
	light_pos_ = view_matrix_.InverseHomo() * light_pos_;
	Vector3d pos = Vector3d(light_pos_.x, light_pos_.y, light_pos_.z);
	Vector3d dir = Vector3d(light_dir().x, light_dir().y, light_dir().z);
	view_matrix_.SetView(pos, dir.Normalize(), Vector3d(0.0, 1.0, 0.0));

	double x_size = aabb.max().x - aabb.min().x;
	double y_size = aabb.max().y - aabb.min().y;
	double z_size = aabb.max().z - aabb.min().z;
	project_matrix_.SetOrtho(x_size / 2, y_size / 2, 0.0, -z_size);
  }
  ShadowBuffer *shadow_buffer() { return shadow_buffer_; }
  void shadow_buffer(int width, int height) { shadow_buffer_ = new ShadowBuffer(width, height); }
  virtual Vector4d light_dir() const = 0;

 protected:
  // normal distribution function
  double NDF(const Vector4d &n, const Vector4d &h, double roughness);
  // fresnel function
  Vector4d FE(const Vector4d &f0, const Vector4d &h, const Vector4d &v);
  // geometry function
  double GE(const Vector4d &n, const Vector4d &v, const Vector4d &l, double roughness);

 private:
  double GESub(double n_dot_v, double roughness);

 protected:
  Vector4d light_pos_, light_color_;
  Vector4d ambient_, diffuse_, specular_;
  LightType type_;
  ShadowBuffer *shadow_buffer_;    // shadow map
  Matrix4d view_matrix_, project_matrix_;
};

class DirectionLight : public Light {
 public:
  DirectionLight(const Vector4d &light_dir)
	  : light_dir_(light_dir.Normalize()) { type_ = LightType::kDir; }
  ~DirectionLight() = default;

  virtual Vector4d Lighting(const Vector4d &normal,
							const Vector4d &pos,
							const Vector4d &view_pos,
							const Vector4d &albedo,
							bool occlude) override;
  virtual Vector4d PBRLighting(const Vector4d &normal,
							   const Vector4d &pos,
							   const Vector4d &view_pos,
							   const Vector4d &albedo,
							   bool occlude) override;

  virtual Vector4d light_dir() const override { return light_dir_; }

 private:
  Vector4d light_dir_;    // inverse direction
};

class PointLight : public Light {
 public:
  PointLight(const Vector4d &light_pos,
			 double constant = 1.0,
			 double linear = 0.09,
			 double quadratic = 0.032)
	  : Light(light_pos),
		constant_(constant),
		linear_(linear),
		quadratic_(quadratic) { type_ = LightType::kPoint; }
  ~PointLight() = default;

  virtual Vector4d Lighting(const Vector4d &normal,
							const Vector4d &pos,
							const Vector4d &view_pos,
							const Vector4d &albedo,
							bool occlude) override;
  virtual Vector4d PBRLighting(const Vector4d &normal,
							   const Vector4d &pos,
							   const Vector4d &view_pos,
							   const Vector4d &albedo,
							   bool occlude) override;

  // do not use
  virtual Vector4d light_dir() const override { return Vector4d{}; }

 private:
  double constant_, linear_, quadratic_;
};

class SpotLight : public Light {
 public:
  SpotLight(const Vector4d &light_pos,
			const Vector4d &spot_dir,
			double inner_cutoff = 12.5,
			double outer_cutoff = 17.5)
	  : Light(light_pos),
		spot_dir_(spot_dir.Normalize()),
		inner_cutoff_(std::cos(inner_cutoff)),
		outer_cutoff_(std::cos(outer_cutoff)) { type_ = LightType::kSpot; }
  ~SpotLight() = default;

  virtual Vector4d Lighting(const Vector4d &normal,
							const Vector4d &pos,
							const Vector4d &view_pos,
							const Vector4d &albedo,
							bool occlude) override;
  virtual Vector4d PBRLighting(const Vector4d &normal,
							   const Vector4d &pos,
							   const Vector4d &view_pos,
							   const Vector4d &albedo,
							   bool occlude) override;

  // do not use
  virtual Vector4d light_dir() const override { return Vector4d{}; }

 private:
  Vector4d spot_dir_;    // inverse direction
  double inner_cutoff_, outer_cutoff_;
};

#endif //SOFTRENDERER_INCLUDE_LIGHT_H_
