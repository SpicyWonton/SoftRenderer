#include "scene.h"
#include <cstdio>
#include <fstream>
#include <sstream>

void Scene::LoadScene() {
  camera_ = new Camera(Vector3d(0, 0, 0), Vector3d(0, 0, 1), Vector3d(0, 1, 0));
  Mesh *mesh;
  Matrix4d model, m_pos, m_rot, m_sca;
  Light *light;
  std::ifstream ifs;
  std::string line, key, x, y, z, w, count, skybox_name;
  std::string base_path = "../assets/scene0/";

  ifs.open(base_path + "scene0_config.txt");
  if (ifs.fail()) {
	printf("Failed to read scene0_config.txt.\n");
	exit(1);
  }

  std::getline(ifs, line);
  std::istringstream iss(line);
  iss >> key;
  if (key != "scene0") {
	printf("scene config file is wrong.\n");
	exit(1);
  }

  while (!ifs.eof()) {
	std::getline(ifs, line);
	std::istringstream iss(line);
	iss >> key;
	if (key == "m") {
	  // read model data
	  iss >> count;
	  for (int i = 0; i < std::stoi(count); i++) {
	    // read obj and texture
	    std::getline(ifs, line);
		std::istringstream model_data(line);
		model_data >> x >> y >> z;
	    mesh = new Mesh();
		mesh->LoadObjFile(base_path + "meshes/" + y + ".obj");
		mesh->LoadAlbedoTexture(base_path + "textures/" + z + "/" + z + "_albedo.png");
		mesh->LoadNormalTexture(base_path + "textures/" + z + "/" + z + "_normal.png");
        // read pos
		std::getline(ifs, line);
		std::istringstream pos(line);
		pos >> key >> x >> y >> z;
		m_pos.SetTranslation(Vector3d(std::stod(x), std::stod(y), std::stod(z)));
		// read rot
		std::getline(ifs, line);
		std::istringstream rot(line);
		rot >> key >> x >> y >> z >> w;
		m_rot.SetRotationAxis(std::stod(w), Vector3d(std::stod(x), std::stod(y), std::stod(z)));
		// read sca
		std::getline(ifs, line);
		std::istringstream sca(line);
		sca >> key >> x >> y >> z;
		m_sca.SetScale(Vector3d(std::stod(x), std::stod(y), std::stod(z)));
		// set model matrix
		model = m_pos * m_rot * m_sca;
		mesh->set_model_matrix(model);
		// load AABB, world coordinate
		mesh->LoadAABB();
		// add mesh to the scene
		meshes_.push_back(mesh);
	  }
	} else if (key == "l") {
	  // read light data
      iss >> count;
      for (int i = 0; i < std::stoi(count); i++) {
		std::getline(ifs, line);
		std::istringstream light_data(line);
		light_data >> key >> x;
		if (x == "dl") {
		  // direction light
		  std::getline(ifs, line);
		  std::istringstream dir(line);
		  dir >> key >> x >> y >> z;
		  light = new DirectionLight(Vector4d(std::stod(x), std::stod(y), std::stod(z), 0.0));
		} else if (x == "pl") {
		  // point light
		  std::getline(ifs, line);
		  std::istringstream pos(line);
		  pos >> key >> x >> y >> z;
		  light = new PointLight(Vector4d(std::stod(x), std::stod(y), std::stod(z)));
		} else if (x == "sl") {
		  // spot light
		  std::getline(ifs, line);
		  std::istringstream pos(line);
		  pos >> key >> x >> y >> z;
		  Vector4d v_pos(std::stod(x), std::stod(y), std::stod(z));
		  std::getline(ifs, line);
		  std::istringstream dir(line);
		  dir >> key >> x >> y >> z;
		  Vector4d v_dir(std::stod(x), std::stod(y), std::stod(z), 0.0);
		  light = new SpotLight(v_pos, v_dir);
		}
		// add light to the scene
		lights_.push_back(light);
      }
	} else if (key == "s") {
	  skybox_ = new Skybox();
	  // read skybox
	  std::getline(ifs, skybox_name);
	  skybox_->LoadSkybox(base_path + "skybox/" + skybox_name);
	}
  }
}

void Scene::UpdateScene(int frame) { }

void Scene::UnLoadScene() {
  if (camera_)
	delete camera_;
  camera_ = nullptr;
  for (int i = 0; i < lights_.size(); i++) {
	if (lights_[i])
	  delete lights_[i];
	lights_[i] = nullptr;
  }
  for (int i = 0; i < meshes_.size(); i++) {
    if (meshes_[i])
      delete meshes_[i];
    meshes_[i] = nullptr;
  }
  if (skybox_)
    delete skybox_;
}