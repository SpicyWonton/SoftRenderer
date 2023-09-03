#ifndef SOFTRENDERER_INCLUDE_SCENE_H_
#define SOFTRENDERER_INCLUDE_SCENE_H_

#include "camera.h"
#include "light.h"
#include "mesh.h"
#include "skybox.h"

class Scene {
 public:
  Scene() : camera_(nullptr), skybox_(nullptr) { lights_.resize(0); meshes_.resize(0); }
  ~Scene() = default;

  void LoadScene();
  void UpdateScene(int frame);
  void UnLoadScene();

  Camera* camera() { return camera_; }
  std::vector<Light*>& lights() { return lights_; }
  std::vector<Mesh*>& meshes() { return meshes_; }
  Skybox* skybox() { return skybox_; }

 private:
  Camera *camera_;
  std::vector<Light*> lights_;
  std::vector<Mesh*> meshes_;
  Skybox *skybox_;
};

#endif //SOFTRENDERER_INCLUDE_SCENE_H_
