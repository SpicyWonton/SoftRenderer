#include "shadow_map.h"
#include "pipeline.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

void ShadowMap::RenderShadowMap(const Matrix4d &viewport_matrix) {
  SetLights();
  for (int k = 0; k < lights_.size(); k++) {
	if (lights_[k]->type() == LightType::kDir) {
	  for (int i = 0; i < meshes_.size(); i++) {
		for (int j = 0; j < meshes_[i]->indices.size(); j += 3) {
		  VertexIn p1, p2, p3;
		  p1 = meshes_[i]->vertices[meshes_[i]->indices[j]];
		  p2 = meshes_[i]->vertices[meshes_[i]->indices[j + 1]];
		  p3 = meshes_[i]->vertices[meshes_[i]->indices[j + 2]];

		  VertexOut v1, v2, v3;
		  v1 = TransformVertex(p1, meshes_[i]->model_matrix, lights_[k]);
		  v2 = TransformVertex(p2, meshes_[i]->model_matrix, lights_[k]);
		  v3 = TransformVertex(p3, meshes_[i]->model_matrix, lights_[k]);

		  v1.pixel_position = viewport_matrix * v1.clip_position;
		  v2.pixel_position = viewport_matrix * v2.clip_position;
		  v3.pixel_position = viewport_matrix * v3.clip_position;

		  SetShadowTexture(v1, v2, v3, lights_[k]);
		}
	  }
	}
	sprintf_s(name_, "shadow_map%d.png", k);
	stbi_write_png(name_,
				   lights_[k]->shadow_buffer()->width(),
				   lights_[k]->shadow_buffer()->height(),
				   3,
				   lights_[k]->shadow_buffer()->shadow_texture(),
				   3 * lights_[k]->shadow_buffer()->width());
  }
}

void ShadowMap::SetLights() {
  for (int i = 0; i < lights_.size(); i++) {
	if (lights_[i]->type() == LightType::kDir) {
	  Vector3d pos = Vector3d(0.0, 0.0, 0.0);
	  Vector3d dir =
		  Vector3d(lights_[i]->light_dir().x, lights_[i]->light_dir().y, lights_[i]->light_dir().z);
	  lights_[i]->view_matrix(pos, dir.Normalize(), Vector3d(0.0, 1.0, 0.0));
	  lights_[i]->project_matrix(meshes_);
	}
  }
}

// 目前没有透视校正，因为只支持方向光阴影
void ShadowMap::SetShadowTexture(const VertexOut &p1,
								 const VertexOut &p2,
								 const VertexOut &p3,
								 Light *light) {
  Vector3d a(p1.pixel_position.x, p1.pixel_position.y, p1.pixel_position.z);
  Vector3d b(p2.pixel_position.x, p2.pixel_position.y, p2.pixel_position.z);
  Vector3d c(p3.pixel_position.x, p3.pixel_position.y, p3.pixel_position.z);

  int x_min = floor(std::min(a.x, std::min(b.x, c.x)));
  int y_min = floor(std::min(a.y, std::min(b.y, c.y)));
  int x_max = ceil(std::max(a.x, std::max(b.x, c.x)));
  int y_max = ceil(std::max(a.y, std::max(b.y, c.y)));

  double alpha, beta, gamma, depth;

  for (int y = y_min; y <= y_max; y++) {
	for (int x = x_min; x <= x_max; x++) {
	  if (Pipeline::InTriangle(Vector2d(x, y), a, b, c, alpha, beta, gamma)) {
		depth = alpha * a.z + beta * b.z + gamma * c.z;
		if (depth < light->shadow_buffer()->GetDepth(x, y))
		  continue;
		light->shadow_buffer()->SetDepth(x, y, depth);
	  }
	}
  }
}

VertexOut ShadowMap::TransformVertex(const VertexIn &in,
									 const Matrix4d &model_matrix,
									 Light *light) {
  VertexOut out;
  out.world_position = model_matrix * in.local_position;
  out.view_position = light->view_matrix() * out.world_position;
  out.clip_position = light->project_matrix() * out.view_position;
  // z range from 0(far) to 1(near)
  out.clip_position.z = (out.clip_position.z + 1.0) * 0.5;
  return out;
}