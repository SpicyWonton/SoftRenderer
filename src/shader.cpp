#include "shader.h"

VertexOut Shader::VertexShader(const VertexIn &in) {
  VertexOut out;
  out.world_position = (*model_matrix_) * in.local_position;
  out.view_position = (*view_matrix_) * out.world_position;
  out.clip_position = (*project_matrix_) * out.view_position;
  out.color = in.color;
  out.normal = in.normal;
  out.texcoord = in.texcoord;
  return out;
}

void Shader::PerspectiveCorrection(VertexOut &in) {
  in.one_div_z = 1 / in.clip_position.w;
  in.world_position *= in.one_div_z;
  in.view_position *= in.one_div_z;
  in.color *= in.one_div_z;
  in.normal *= in.one_div_z;
  in.texcoord *= in.one_div_z;
}

void Shader::TBN_matrix(const VertexIn &a, const VertexIn &b, const VertexIn &c) {
  Vector3d ab = b.local_position - a.local_position;
  Vector3d ac = c.local_position - a.local_position;
  Vector3d N = ab.Cross(ac).Normalize(), T, B;
  double delta_u1 = b.texcoord.x - a.texcoord.x;
  double delta_u2 = c.texcoord.x - a.texcoord.x;
  double delta_v1 = b.texcoord.y - a.texcoord.y;
  double delta_v2 = c.texcoord.y - a.texcoord.y;
  double denom = 1.0 / (delta_u1 * delta_v2 - delta_u2 * delta_v1);
  // construct T and B axis
  T.x = denom * (delta_v2 * ab.x - delta_v1 * ac.x);
  T.y = denom * (delta_v2 * ab.y - delta_v1 * ac.y);
  T.z = denom * (delta_v2 * ab.z - delta_v1 * ac.z);

  B.x = denom * (delta_u1 * ac.x - delta_u2 * ab.x);
  B.y = denom * (delta_u1 * ac.y - delta_u2 * ab.y);
  B.z = denom * (delta_u1 * ac.z - delta_u2 * ab.z);
  // Schmidt orthogonalization
  T = (T - T.Dot(N) * N).Normalize();
  B = (B - B.Dot(N) * N - B.Dot(T) * T).Normalize();
  // Update TBN matrix
  TBN_matrix_.SetMatrix(T, B, N);
}

void Shader::set_model_normal_matrix() {
  model_normal_matrix_ = (*model_matrix_).AdjointMatrix33().Transpose33();
}

Vector4d PhongShader::FragmentShader(const VertexOut &in) {
  Vector4d color;
  // Vector4d normal = (model_normal_matrix_ * in.normal).Normalize();
  Vector4d normal = normal_texture_->Sample(in.texcoord);
  normal /= 255.0;
  normal = 2.0 * normal - Vector4d(1.0, 1.0, 1.0, 0.0);
  normal = (model_normal_matrix_ * TBN_matrix_ * normal).Normalize();

  Vector4d view_pos = *view_pos_;
  Vector4d tex_color = albedo_texture_->Sample(in.texcoord);
  Vector4d light_pixel_pos;
  double depth;
  for (int i = 0; i < lights_.size(); i++) {
	light_pixel_pos = (*viewport_matrix_) * lights_[i]->project_matrix() * lights_[i]->view_matrix()
		* in.world_position;
	depth = (light_pixel_pos.z + 1.0) * 0.5;
	if (depth + 0.1 < lights_[i]->shadow_buffer()->GetDepth(light_pixel_pos.x, light_pixel_pos.y))
	  color += lights_[i]->Lighting(normal, in.world_position, view_pos, tex_color, true);
	else
	  color += lights_[i]->Lighting(normal, in.world_position, view_pos, tex_color, false);
  }
  Clamp(color, 0.0, 255.0);

  return color;
}

Vector4d LineShader::FragmentShader(const VertexOut &in) {
  return Vector4d(255.0, 255.0, 255.0, 1.0);
}

Vector4d PBRShader::FragmentShader(const VertexOut &in) {
  Vector4d color;
  Vector4d normal = (model_normal_matrix_ * in.normal).Normalize();
  Vector4d tex_color = albedo_texture_->Sample(in.texcoord);
  for (int i = 0; i < lights_.size(); i++) {
	  color += lights_[i]->PBRLighting(normal, in.world_position, *view_pos_, tex_color, false);
  }
  Clamp(color, 0.0, 255.0);

  return color;
}
