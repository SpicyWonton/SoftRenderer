#include "pipeline.h"
#include "shader.h"

Pipeline::Pipeline(int width, int height) : width_(width), height_(height) {
  shader_ = new PhongShader();
  shadow_map_ = new ShadowMap();
  front_buffer_ = new FrameBuffer(width, height);
  back_buffer_ = new FrameBuffer(width, height);
  viewport_matrix_.SetViewport(0, 0, width, height);
  shader_->set_viewport_matrix(&viewport_matrix_);
}

Pipeline::~Pipeline() {
  if (shader_) delete shader_;
  if (shadow_map_) delete shadow_map_;
  if (front_buffer_) delete front_buffer_;
  if (back_buffer_) delete back_buffer_;
  shader_ = nullptr;
  shadow_map_ = nullptr;
  front_buffer_ = nullptr;
  back_buffer_ = nullptr;
}

void Pipeline::ClearBuffer(const Vector4d &color) {
  back_buffer_->ClearBuffer(color);
}

void Pipeline::SwapBuffer() {
  FrameBuffer *tmp = front_buffer_;
  front_buffer_ = back_buffer_;
  back_buffer_ = tmp;
}

void Pipeline::SwitchMode(RenderMode mode) {
  switch (mode) {
    case RenderMode::kLine:
	  delete shader_;
	  shader_ = new LineShader();
	  break;
	case RenderMode::kFull:
	  delete shader_;
	  shader_ = new PhongShader();
	  break;
	case RenderMode::kPBR:
	  delete shader_;
	  shader_ = new PBRShader();
	  break;
  }
  shader_->set_project_matrix(project_matrix_);
  shader_->set_viewport_matrix(&viewport_matrix_);
}

void Pipeline::RenderShadowMap() {
  shadow_map_->RenderShadowMap(viewport_matrix_);
}

void Pipeline::Draw(RenderMode mode) {
  if (meshes_.empty()) return;

  for (int i = 0; i < meshes_.size(); i++) {
	shader_->albedo_texture(&(meshes_[i]->albedo_texture));
	shader_->normal_texture(&(meshes_[i]->normal_texture));
	shader_->set_model_matrix(&(meshes_[i]->model_matrix));
	for (int j = 0; j < meshes_[i]->indices.size(); j += 3) {
	  VertexIn p1, p2, p3;
	  p1 = meshes_[i]->vertices[meshes_[i]->indices[j]];
	  p2 = meshes_[i]->vertices[meshes_[i]->indices[j + 1]];
	  p3 = meshes_[i]->vertices[meshes_[i]->indices[j + 2]];
	  // construct TBN matrix for normal mapping
	  shader_->TBN_matrix(p1, p2, p3);

	  VertexOut v1, v2, v3;
	  v1 = shader_->VertexShader(p1);
	  v2 = shader_->VertexShader(p2);
	  v3 = shader_->VertexShader(p3);

	  if (BackFaceCulling(v1.view_position, v2.view_position, v3.view_position))
		continue;

	  shader_->PerspectiveCorrection(v1);
	  shader_->PerspectiveCorrection(v2);
	  shader_->PerspectiveCorrection(v3);

	  std::vector<VertexOut> in_vertices = HomogeneousClipping(v1, v2, v3);
	  int size = in_vertices.size();
	  for (int k = 0; k < size; k++) {
		PerspectiveDivision(in_vertices[k]);
		in_vertices[k].pixel_position = viewport_matrix_ * in_vertices[k].clip_position;
	  }
	  for (int k = 0; k < size - 2; k++) {
		int index0 = 0;
		int index1 = k + 1;
		int index2 = k + 2;
		if (mode == RenderMode::kFull || mode == RenderMode::kPBR) {
		  DrawTriangle(in_vertices[index0], in_vertices[index1], in_vertices[index2]);
		} else {
		  DrawLine(in_vertices[index0], in_vertices[index1]);
		  DrawLine(in_vertices[index1], in_vertices[index2]);
		  DrawLine(in_vertices[index2], in_vertices[index0]);
		}
	  }
	}
  }

  // DrawSkybox(mode);
}

// back face will return true
bool Pipeline::BackFaceCulling(const Vector4d &v1, const Vector4d &v2, const Vector4d &v3) {
  Vector3d a(v1.x, v1.y, v1.z);
  Vector3d b(v2.x, v2.y, v2.z);
  Vector3d c(v3.x, v3.y, v3.z);
  Vector3d ab = b - a;
  Vector3d ac = c - a;
  Vector3d ae = -a;
  Vector3d normal = ab.Cross(ac);
  return normal.Dot(ae) < 0;
}

// homogeneous clipping
std::vector<VertexOut> Pipeline::HomogeneousClipping(const VertexOut &p1,
													 const VertexOut &p2,
													 const VertexOut &p3) {
  int num_vertex = 3;
  std::vector<VertexOut> vertices = {p1, p2, p3};
  std::vector<VertexOut> in_vertices0, in_vertices1, in_vertices2, in_vertices3, in_vertices4,
	  in_vertices5, in_vertices6;

  // clip
  if (num_vertex > 0)
	in_vertices0 = ClipWithPlane(ClipPlane::kWZero, num_vertex, vertices);
  if (num_vertex > 0)
	in_vertices1 = ClipWithPlane(ClipPlane::kNear, num_vertex, in_vertices0);
  if (num_vertex > 0)
	in_vertices2 = ClipWithPlane(ClipPlane::kFar, num_vertex, in_vertices1);
  if (num_vertex > 0)
	in_vertices3 = ClipWithPlane(ClipPlane::kLeft, num_vertex, in_vertices2);
  if (num_vertex > 0)
	in_vertices4 = ClipWithPlane(ClipPlane::kRight, num_vertex, in_vertices3);
  if (num_vertex > 0)
	in_vertices5 = ClipWithPlane(ClipPlane::kTop, num_vertex, in_vertices4);
  if (num_vertex > 0)
	in_vertices6 = ClipWithPlane(ClipPlane::kBottom, num_vertex, in_vertices5);

  return in_vertices6;
}

// Clip a convex polygon with plane
std::vector<VertexOut> Pipeline::ClipWithPlane(ClipPlane type,
											   int &num_vertex,
											   const std::vector<VertexOut> &vertices) {
  std::vector<VertexOut> in_vertices;
  VertexOut vertex;
  int prev = num_vertex - 1;
  int pdot = 0, idot, dot;
  double t = 0;
  double w1, w2, x1, x2, y1, y2, z1, z2;

  switch (type) {
	// drop the point whose w is small than zero
	case ClipPlane::kWZero: idot = vertices[0].clip_position.w > 0 ? 1 : -1;
	  for (int i = 0; i < num_vertex; i++) {
		dot = vertices[i].clip_position.w > 0 ? 1 : -1;
		// intersect with the plane
		if (dot * pdot < 0) {
		  w1 = vertices[prev].clip_position.w;
		  w2 = vertices[i].clip_position.w;
		  t = w1 / (w1 - w2);
		  // lerp
		  vertex = VertexOut::Lerp(vertices[prev], vertices[i], t);
		  // add vertex to in_vertices
		  in_vertices.push_back(vertex);
		}
		// current point is in
		if (dot > 0) {
		  in_vertices.push_back(vertices[i]);
		}
		// update index
		prev = i;
		pdot = dot;
	  }
	  // handle the last and the first
	  if (pdot * idot < 0) {
		w1 = vertices[prev].clip_position.w;
		w2 = vertices[0].clip_position.w;
		t = w1 / (w1 - w2);
		// lerp
		vertex = VertexOut::Lerp(vertices[prev], vertices[0], t);
		// add vertex to in_vertices
		in_vertices.push_back(vertex);
	  }
	  num_vertex = in_vertices.size();
	  break;
	case ClipPlane::kLeft:
	  idot = vertices[0].clip_position.x > -vertices[0].clip_position.w ? 1 : -1;
	  for (int i = 0; i < num_vertex; i++) {
		dot = vertices[i].clip_position.x > -vertices[i].clip_position.w ? 1 : -1;
		// intersect with the plane
		if (dot * pdot < 0) {
		  w1 = vertices[prev].clip_position.w;
		  x1 = vertices[prev].clip_position.x;
		  w2 = vertices[i].clip_position.w;
		  x2 = vertices[i].clip_position.x;
		  t = (w1 + x1) / ((w1 + x1) - (w2 + x2));
		  // lerp
		  vertex = VertexOut::Lerp(vertices[prev], vertices[i], t);
		  // add vertex to in_vertices
		  in_vertices.push_back(vertex);
		}
		// current point is in
		if (dot > 0) {
		  in_vertices.push_back(vertices[i]);
		}
		// update index
		prev = i;
		pdot = dot;
	  }
	  // handle the last and the first
	  if (pdot * idot < 0) {
		w1 = vertices[prev].clip_position.w;
		x1 = vertices[prev].clip_position.x;
		w2 = vertices[0].clip_position.w;
		x2 = vertices[0].clip_position.x;
		t = (w1 + x1) / ((w1 + x1) - (w2 + x2));
		// lerp
		vertex = VertexOut::Lerp(vertices[prev], vertices[0], t);
		// add vertex to in_vertices
		in_vertices.push_back(vertex);
	  }
	  num_vertex = in_vertices.size();
	  break;
	case ClipPlane::kRight:
	  idot = vertices[0].clip_position.x < vertices[0].clip_position.w ? 1 : -1;
	  for (int i = 0; i < num_vertex; i++) {
		dot = vertices[i].clip_position.x < vertices[i].clip_position.w ? 1 : -1;
		// intersect with the plane
		if (dot * pdot < 0) {
		  w1 = vertices[prev].clip_position.w;
		  x1 = vertices[prev].clip_position.x;
		  w2 = vertices[i].clip_position.w;
		  x2 = vertices[i].clip_position.x;
		  t = (w1 - x1) / ((w1 - x1) - (w2 - x2));
		  // lerp
		  vertex = VertexOut::Lerp(vertices[prev], vertices[i], t);
		  // add vertex to in_vertices
		  in_vertices.push_back(vertex);
		}
		// current point is in
		if (dot > 0) {
		  in_vertices.push_back(vertices[i]);
		}
		// update index
		prev = i;
		pdot = dot;
	  }
	  // handle the last and the first
	  if (pdot * idot < 0) {
		w1 = vertices[prev].clip_position.w;
		x1 = vertices[prev].clip_position.x;
		w2 = vertices[0].clip_position.w;
		x2 = vertices[0].clip_position.x;
		t = (w1 - x1) / ((w1 - x1) - (w2 - x2));
		// lerp
		vertex = VertexOut::Lerp(vertices[prev], vertices[0], t);
		// add vertex to in_vertices
		in_vertices.push_back(vertex);
	  }
	  num_vertex = in_vertices.size();
	  break;
	case ClipPlane::kNear:
	  idot = vertices[0].clip_position.z > -vertices[0].clip_position.w ? 1 : -1;
	  for (int i = 0; i < num_vertex; i++) {
		dot = vertices[i].clip_position.z > -vertices[i].clip_position.w ? 1 : -1;
		// intersect with the plane
		if (dot * pdot < 0) {
		  w1 = vertices[prev].clip_position.w;
		  z1 = vertices[prev].clip_position.z;
		  w2 = vertices[i].clip_position.w;
		  z2 = vertices[i].clip_position.z;
		  t = (w1 + z1) / ((w1 + z1) - (w2 + z2));
		  // lerp
		  vertex = VertexOut::Lerp(vertices[prev], vertices[i], t);
		  // add vertex to in_vertices
		  in_vertices.push_back(vertex);
		}
		// current point is in
		if (dot > 0) {
		  in_vertices.push_back(vertices[i]);
		}
		// update index
		prev = i;
		pdot = dot;
	  }
	  // handle the last and the first
	  if (pdot * idot < 0) {
		w1 = vertices[prev].clip_position.w;
		z1 = vertices[prev].clip_position.z;
		w2 = vertices[0].clip_position.w;
		z2 = vertices[0].clip_position.z;
		t = (w1 + z1) / ((w1 + z1) - (w2 + z2));
		// lerp
		vertex = VertexOut::Lerp(vertices[prev], vertices[0], t);
		// add vertex to in_vertices
		in_vertices.push_back(vertex);
	  }
	  num_vertex = in_vertices.size();
	  break;
	case ClipPlane::kFar: idot = vertices[0].clip_position.z < vertices[0].clip_position.w ? 1 : -1;
	  for (int i = 0; i < num_vertex; i++) {
		dot = vertices[i].clip_position.z < vertices[i].clip_position.w ? 1 : -1;
		// intersect with the plane
		if (dot * pdot < 0) {
		  w1 = vertices[prev].clip_position.w;
		  z1 = vertices[prev].clip_position.z;
		  w2 = vertices[i].clip_position.w;
		  z2 = vertices[i].clip_position.z;
		  t = (w1 - z1) / ((w1 - z1) - (w2 - z2));
		  // lerp
		  vertex = VertexOut::Lerp(vertices[prev], vertices[i], t);
		  // add vertex to in_vertices
		  in_vertices.push_back(vertex);
		}
		// current point is in
		if (dot > 0) {
		  in_vertices.push_back(vertices[i]);
		}
		// update index
		prev = i;
		pdot = dot;
	  }
	  // handle the last and the first
	  if (pdot * idot < 0) {
		w1 = vertices[prev].clip_position.w;
		z1 = vertices[prev].clip_position.z;
		w2 = vertices[0].clip_position.w;
		z2 = vertices[0].clip_position.z;
		t = (w1 - z1) / ((w1 - z1) - (w2 - z2));
		// lerp
		vertex = VertexOut::Lerp(vertices[prev], vertices[0], t);
		// add vertex to in_vertices
		in_vertices.push_back(vertex);
	  }
	  num_vertex = in_vertices.size();
	  break;
	case ClipPlane::kTop: idot = vertices[0].clip_position.y < vertices[0].clip_position.w ? 1 : -1;
	  for (int i = 0; i < num_vertex; i++) {
		dot = vertices[i].clip_position.y < vertices[i].clip_position.w ? 1 : -1;
		// intersect with the plane
		if (dot * pdot < 0) {
		  w1 = vertices[prev].clip_position.w;
		  y1 = vertices[prev].clip_position.y;
		  w2 = vertices[i].clip_position.w;
		  y2 = vertices[i].clip_position.y;
		  t = (w1 - y1) / ((w1 - y1) - (w2 - y2));
		  // lerp
		  vertex = VertexOut::Lerp(vertices[prev], vertices[i], t);
		  // add vertex to in_vertices
		  in_vertices.push_back(vertex);
		}
		// current point is in
		if (dot > 0) {
		  in_vertices.push_back(vertices[i]);
		}
		// update index
		prev = i;
		pdot = dot;
	  }
	  // handle the last and the first
	  if (pdot * idot < 0) {
		w1 = vertices[prev].clip_position.w;
		y1 = vertices[prev].clip_position.y;
		w2 = vertices[0].clip_position.w;
		y2 = vertices[0].clip_position.y;
		t = (w1 - y1) / ((w1 - y1) - (w2 - y2));
		// lerp
		vertex = VertexOut::Lerp(vertices[prev], vertices[0], t);
		// add vertex to in_vertices
		in_vertices.push_back(vertex);
	  }
	  num_vertex = in_vertices.size();
	  break;
	case ClipPlane::kBottom:
	  idot = vertices[0].clip_position.y > -vertices[0].clip_position.w ? 1 : -1;
	  for (int i = 0; i < num_vertex; i++) {
		dot = vertices[i].clip_position.y > -vertices[i].clip_position.w ? 1 : -1;
		// intersect with the plane
		if (dot * pdot < 0) {
		  w1 = vertices[prev].clip_position.w;
		  y1 = vertices[prev].clip_position.y;
		  w2 = vertices[i].clip_position.w;
		  y2 = vertices[i].clip_position.y;
		  t = (w1 + y1) / ((w1 + y1) - (w2 + y2));
		  // lerp
		  vertex = VertexOut::Lerp(vertices[prev], vertices[i], t);
		  // add vertex to in_vertices
		  in_vertices.push_back(vertex);
		}
		// current point is in
		if (dot > 0) {
		  in_vertices.push_back(vertices[i]);
		}
		// update index
		prev = i;
		pdot = dot;
	  }
	  // handle the last and the first
	  if (pdot * idot < 0) {
		w1 = vertices[prev].clip_position.w;
		y1 = vertices[prev].clip_position.y;
		w2 = vertices[0].clip_position.w;
		y2 = vertices[0].clip_position.y;
		t = (w1 + y1) / ((w1 + y1) - (w2 + y2));
		// lerp
		vertex = VertexOut::Lerp(vertices[prev], vertices[0], t);
		// add vertex to in_vertices
		in_vertices.push_back(vertex);
	  }
	  num_vertex = in_vertices.size();
	  break;
  }

  return in_vertices;
}

void Pipeline::PerspectiveDivision(VertexOut &v) {
  v.clip_position /= v.clip_position.w;
  v.clip_position.w = 1.0;
  // map [-1,1] to [0,1]
  v.clip_position.z = (v.clip_position.z + 1.0) * 0.5;
}

void Pipeline::DrawLine(const VertexOut &p1, const VertexOut &p2) {
  int ix0 = static_cast<int>(floor(p1.pixel_position.x));
  int iy0 = static_cast<int>(floor(p1.pixel_position.y));
  int ix1 = static_cast<int>(floor(p2.pixel_position.x));
  int iy1 = static_cast<int>(floor(p2.pixel_position.y));
  double z0 = p1.pixel_position.z;
  double z1 = p2.pixel_position.z;
  int tmp = 1;           // 用于处理m小于0
  bool steep = false;    // 用于处理是否陡峭
  // 处理m绝对值大于1的情况
  if (abs(iy1 - iy0) > abs(ix1 - ix0)) {
	std::swap(ix0, iy0);
	std::swap(ix1, iy1);
	steep = true;
  }
  // 处理x变小的情况
  if (ix0 > ix1) {
	std::swap(ix0, ix1);
	std::swap(iy0, iy1);
	std::swap(z0, z1);
  }
  // 处理m小于0的情况
  if (iy1 < iy0) {
	tmp = -1;
  }
  // Bresenham算法
  Vector4d color;
  VertexOut curr;
  int delta_x = ix1 - ix0, delta_y = iy1 - iy0;
  double depth, t;
  for (int x = ix0, y = iy0, eps = 0; x <= ix1; x++) {
	if (steep) {
	  // depth test
	  t = static_cast<double>(x - ix0) / static_cast<double>(delta_x);
	  depth = z0 * (1.0 - t) + z1 * t;
	  if (depth < back_buffer_->GetDepth(y, x)) {
		back_buffer_->SetDepth(y, x, depth);
		// shading
		curr.pixel_position.x = y;
		curr.pixel_position.y = x;
		color = shader_->FragmentShader(curr);
		back_buffer_->DrawPixel(y, x, color);
	  }
	} else {
	  // depth test
	  t = static_cast<double>(x - ix0) / static_cast<double>(delta_x);
	  depth = z0 * (1.0 - t) + z1 * t;
	  if (depth < back_buffer_->GetDepth(x, y)) {
		back_buffer_->SetDepth(x, y, depth);
		// shading
		curr.pixel_position.x = x;
		curr.pixel_position.y = y;
		color = shader_->FragmentShader(curr);
		back_buffer_->DrawPixel(x, y, color);
	  }
	}
	if (2 * tmp * (eps + delta_y) < delta_x) {
	  eps += delta_y;
	} else {
	  eps += delta_y - tmp * delta_x;
	  y += tmp;
	}
  }
}

void Pipeline::DrawTriangle(const VertexOut &p1, const VertexOut &p2, const VertexOut &p3) {
  Vector3d a(p1.pixel_position.x, p1.pixel_position.y, p1.pixel_position.z);
  Vector3d b(p2.pixel_position.x, p2.pixel_position.y, p2.pixel_position.z);
  Vector3d c(p3.pixel_position.x, p3.pixel_position.y, p3.pixel_position.z);

  int x_min = floor(std::min(a.x, std::min(b.x, c.x)));
  int y_min = floor(std::min(a.y, std::min(b.y, c.y)));
  int x_max = ceil(std::max(a.x, std::max(b.x, c.x)));
  int y_max = ceil(std::max(a.y, std::max(b.y, c.y)));

  double alpha, beta, gamma, depth;
  VertexOut curr;
  Vector4d color;
  double w;

  for (int y = y_min; y <= y_max; y++) {
	for (int x = x_min; x <= x_max; x++) {
	  if (InTriangle(Vector2d(x, y), a, b, c, alpha, beta, gamma)) {
		// depth test
		depth = alpha * a.z + beta * b.z + gamma * c.z;
		if (depth > back_buffer_->GetDepth(x, y)) continue;
		back_buffer_->SetDepth(x, y, depth);
		// lerp
		curr.world_position =
			alpha * p1.world_position + beta * p2.world_position + gamma * p3.world_position;
		curr.view_position =
			alpha * p1.view_position + beta * p2.view_position + gamma * p3.view_position;
		curr.normal = alpha * p1.normal + beta * p2.normal + gamma * p3.normal;
		curr.texcoord = alpha * p1.texcoord + beta * p2.texcoord + gamma * p3.texcoord;
		curr.one_div_z = alpha * p1.one_div_z + beta * p2.one_div_z + gamma * p3.one_div_z;
		// restore
		w = 1.0 / curr.one_div_z;
		curr.world_position *= w;
		curr.view_position *= w;
		curr.texcoord *= w;
		curr.color *= w;
		curr.normal *= w;
		// fragment shader
		color = shader_->FragmentShader(curr);
		back_buffer_->DrawPixel(x, y, color);
	  }
	}
  }
}

void Pipeline::DrawSkybox(RenderMode mode) {
  if (mode == RenderMode::kLine)
	return;
  for (int i = 0, j = 0; i < skybox_->indices().size(); i += 3) {
    if (i % 6 == 0 && i != 0)
      j++;
	SkyBoxVertex v1, v2, v3;
	v1 = skybox_->vertices()[skybox_->indices()[i]];
	v2 = skybox_->vertices()[skybox_->indices()[i + 1]];
	v3 = skybox_->vertices()[skybox_->indices()[i + 2]];
	v1.pos = skybox_->TransformPos(viewport_matrix_, *project_matrix_, *view_matrix_, v1.pos);
	v2.pos = skybox_->TransformPos(viewport_matrix_, *project_matrix_, *view_matrix_, v2.pos);
	v3.pos = skybox_->TransformPos(viewport_matrix_, *project_matrix_, *view_matrix_, v3.pos);
	DrawSkyboxTriangle(v1, v2, v3, j);
  }
}

void Pipeline::DrawSkyboxTriangle(const SkyBoxVertex &v1,
								  const SkyBoxVertex &v2,
								  const SkyBoxVertex &v3, int index) {
  Vector3d a(v1.pos.x, v1.pos.y, v1.pos.z);
  Vector3d b(v2.pos.x, v2.pos.y, v2.pos.z);
  Vector3d c(v3.pos.x, v3.pos.y, v3.pos.z);

  int x_min = floor(std::min(a.x, std::min(b.x, c.x)));
  int y_min = floor(std::min(a.y, std::min(b.y, c.y)));
  int x_max = ceil(std::max(a.x, std::max(b.x, c.x)));
  int y_max = ceil(std::max(a.y, std::max(b.y, c.y)));

  double alpha, beta, gamma, depth;
  Vector4d color;

  for (int y = y_min; y <= y_max; y++) {
	for (int x = x_min; x <= x_max; x++) {
	  if (InTriangle(Vector2d(x, y), a, b, c, alpha, beta, gamma)) {
		if (back_buffer_->GetDepth(x, y) >= 1.0) {
		  Vector2d uv = alpha * v1.tex + beta * v2.tex + gamma * v3.tex;
		  switch (index) {
		    case 0:
		      color = skybox_->Sample(uv, Face::kFront);
		      break;
		    case 1:
		      color = skybox_->Sample(uv, Face::kBack);
			  break;
		    case 2:
		      color = skybox_->Sample(uv, Face::kLeft);
			  break;
		    case 3:
		      color = skybox_->Sample(uv, Face::kRight);
			  break;
		    case 4:
		      color = skybox_->Sample(uv, Face::kUp);
			  break;
		    case 5:
		      color = skybox_->Sample(uv, Face::kDown);
			  break;
		  }
		  back_buffer_->DrawPixel(x, y, color);
		}
	  }
	}
  }
}

bool Pipeline::InTriangle(const Vector2d &p, const Vector3d &a,
						  const Vector3d &b, const Vector3d &c, double &alpha,
						  double &beta, double &gamma) {
  double x_ba = b.x - a.x;
  double x_ca = c.x - a.x;
  double x_pa = p.x - a.x;
  double y_ba = b.y - a.y;
  double y_ca = c.y - a.y;
  double y_pa = p.y - a.y;

  double den = x_ba * y_ca - x_ca * y_ba;
  double num_beta = x_pa * y_ca - x_ca * y_pa;
  double num_gamma = x_ba * y_pa - x_pa * y_ba;

  beta = num_beta / den;
  gamma = num_gamma / den;
  alpha = 1.0 - beta - gamma;

  if (alpha >= 0 && beta >= 0 && gamma >= 0) {
	return true;
  } else {
	return false;
  }
}