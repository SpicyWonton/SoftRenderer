#include "skybox.h"

Skybox::Skybox() {
  skybox_.resize(6);
  vertices_.resize(24);
  indices_.resize(36);
  // front
  vertices_[0].pos = Vector4d(-0.5, 0.5, -0.5, 1.0);
  vertices_[0].tex = Vector2d(0.0, 1.0);

  vertices_[1].pos = Vector4d(-0.5, -0.5, -0.5, 1.0);
  vertices_[1].tex = Vector2d(0.0, 0.0);

  vertices_[2].pos = Vector4d(0.5, -0.5, -0.5, 1.0);
  vertices_[2].tex = Vector2d(1.0, 0.0);

  vertices_[3].pos = Vector4d(0.5, 0.5, -0.5, 1.0);
  vertices_[3].tex = Vector2d(1.0, 1.0);
  //back
  vertices_[4].pos = Vector4d(0.5, 0.5, 0.5, 1.0);
  vertices_[4].tex = Vector2d(0.0, 1.0);

  vertices_[5].pos = Vector4d(0.5, -0.5, 0.5, 1.0);
  vertices_[5].tex = Vector2d(0.0, 0.0);

  vertices_[6].pos = Vector4d(-0.5, -0.5, 0.5, 1.0);
  vertices_[6].tex = Vector2d(1.0, 0.0);

  vertices_[7].pos = Vector4d(-0.5, 0.5, 0.5, 1.0);
  vertices_[7].tex = Vector2d(1.0, 1.0);
  // left
  vertices_[8].pos = Vector4d(-0.5, 0.5, 0.5, 1.0);
  vertices_[8].tex = Vector2d(0.0, 1.0);

  vertices_[9].pos = Vector4d(-0.5, -0.5, 0.5, 1.0);
  vertices_[9].tex = Vector2d(0.0, 0.0);

  vertices_[10].pos = Vector4d(-0.5, -0.5, -0.5, 1.0);
  vertices_[10].tex = Vector2d(1.0, 0.0);

  vertices_[11].pos = Vector4d(-0.5, 0.5, -0.5, 1.0);
  vertices_[11].tex = Vector2d(1.0, 1.0);
  // right
  vertices_[12].pos = Vector4d(0.5, 0.5, -0.5, 1.0);
  vertices_[12].tex = Vector2d(0.0, 1.0);

  vertices_[13].pos = Vector4d(0.5, -0.5, -0.5, 1.0);
  vertices_[13].tex = Vector2d(0.0, 0.0);

  vertices_[14].pos = Vector4d(0.5, -0.5, 0.5, 1.0);
  vertices_[14].tex = Vector2d(1.0, 0.0);

  vertices_[15].pos = Vector4d(0.5, 0.5, 0.5, 1.0);
  vertices_[15].tex = Vector2d(1.0, 1.0);
  // up
  vertices_[16].pos = Vector4d(-0.5, 0.5, 0.5, 1.0);
  vertices_[16].tex = Vector2d(0.0, 1.0);

  vertices_[17].pos = Vector4d(-0.5, 0.5, -0.5, 1.0);
  vertices_[17].tex = Vector2d(0.0, 0.0);

  vertices_[18].pos = Vector4d(0.5, 0.5, -0.5, 1.0);
  vertices_[18].tex = Vector2d(1.0, 0.0);

  vertices_[19].pos = Vector4d(0.5, 0.5, 0.5, 1.0);
  vertices_[19].tex = Vector2d(1.0, 1.0);
  // down
  vertices_[20].pos = Vector4d(-0.5, -0.5, -0.5, 1.0);
  vertices_[20].tex = Vector2d(0.0, 1.0);

  vertices_[21].pos = Vector4d(-0.5, -0.5, 0.5, 1.0);
  vertices_[21].tex = Vector2d(0.0, 0.0);

  vertices_[22].pos = Vector4d(0.5, -0.5, 0.5, 1.0);
  vertices_[22].tex = Vector2d(1.0, 0.0);

  vertices_[23].pos = Vector4d(0.5, -0.5, -0.5, 1.0);
  vertices_[23].tex = Vector2d(1.0, 1.0);
  // init indices
  // front
  indices_[0] = 0;
  indices_[1] = 1;
  indices_[2] = 2;

  indices_[3] = 2;
  indices_[4] = 3;
  indices_[5] = 0;
  // back
  indices_[6] = 4;
  indices_[7] = 5;
  indices_[8] = 6;

  indices_[9] = 6;
  indices_[10] = 7;
  indices_[11] = 4;
  // left
  indices_[12] = 8;
  indices_[13] = 9;
  indices_[14] = 10;

  indices_[15] = 10;
  indices_[16] = 11;
  indices_[17] = 8;
  // right
  indices_[18] = 12;
  indices_[19] = 13;
  indices_[20] = 14;

  indices_[21] = 14;
  indices_[22] = 15;
  indices_[23] = 12;
  // up
  indices_[24] = 16;
  indices_[25] = 17;
  indices_[26] = 18;

  indices_[27] = 18;
  indices_[28] = 19;
  indices_[29] = 16;
  // down
  indices_[30] = 20;
  indices_[31] = 21;
  indices_[32] = 22;

  indices_[33] = 22;
  indices_[34] = 23;
  indices_[35] = 20;
}

void Skybox::LoadSkybox(const std::string &path) {
  skybox_[0].LoadImage((path + "_ft.png").c_str());
  skybox_[1].LoadImage((path + "_bk.png").c_str());
  skybox_[2].LoadImage((path + "_lf.png").c_str());
  skybox_[3].LoadImage((path + "_rt.png").c_str());
  skybox_[4].LoadImage((path + "_up.png").c_str());
  skybox_[5].LoadImage((path + "_dn.png").c_str());
}

Vector4d Skybox::Sample(const Vector2d &uv, Face face) {
  switch (face) {
	case Face::kFront:return skybox_[0].Sample(uv);
	case Face::kBack: return skybox_[1].Sample(uv);
	case Face::kLeft: return skybox_[2].Sample(uv);
	case Face::kRight: return skybox_[3].Sample(uv);
	case Face::kUp: return skybox_[4].Sample(uv);
	case Face::kDown: return skybox_[5].Sample(uv);
  }
}

Vector4d Skybox::TransformPos(const Matrix4d &viewport,
							  const Matrix4d &project,
							  const Matrix4d &view,
							  const Vector4d &pos) {
  Vector4d res = project * (view * pos);
  res /= res.w;
  res.z = 1.0;
  res.w = 1.0;
  res = viewport * res;
  return res;
}