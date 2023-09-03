#include "mesh.h"

#include <cstdio>
#include <fstream>
#include <sstream>

void Mesh::LoadObjFile(const std::string &path) {
  std::ifstream ifs;
  std::string line, key, x, y, z;
  std::vector<Vector4d> position;
  std::vector<Vector2d> texcoord;
  std::vector<Vector4d> normal;
  std::vector<std::string> split_index;
  int index[3];

  ifs.open(path);
  if (ifs.fail()) printf("Failed to load obj file.\n");

  while (std::getline(ifs, line)) {
	std::istringstream iss(line);
	iss >> key;
	if (key == "v") {
	  iss >> x >> y >> z;
	  position.emplace_back(std::stod(x), std::stod(y), std::stod(z));
	} else if (key == "vt") {
	  iss >> x >> y;
	  texcoord.emplace_back(std::stod(x), std::stod(y));
	} else if (key == "vn") {
	  iss >> x >> y >> z;
	  normal.emplace_back(std::stod(x), std::stod(y), std::stod(z), 0);
	} else if (key == "f") {
	  while (iss >> x) {
		split_index = Split(x);
		for (int i = 0; i < 3; i++) {
		  index[i] = std::stoi(split_index[i]);
		}
		indices.push_back(vertices.size());
		vertices.emplace_back(position[index[0] - 1], Vector4d(1.0, 1.0, 1.0),
							  normal[index[2] - 1], texcoord[index[1] - 1]);
	  }
	}
  }

  ifs.close();
}

void Mesh::LoadAlbedoTexture(const std::string &path) {
  albedo_texture.LoadImage(path.c_str());
}

void Mesh::LoadNormalTexture(const std::string &path) {
  normal_texture.LoadImage(path.c_str());
}

// set AABB in world coordinate
void Mesh::LoadAABB() {
  for (int i = 0; i < vertices.size(); i++) {
	aabb_ = AABB::Union(aabb_, model_matrix * vertices[i].local_position);
  }
}

std::vector<std::string> Mesh::Split(const std::string &str, const std::string &delimiter) {
  std::vector<std::string> split_str;
  std::size_t pos1 = str.find_first_not_of(delimiter, 0);
  std::size_t pos2 = str.find_first_of(delimiter, pos1);
  while (pos1 != std::string::npos || pos2 != std::string::npos) {
	split_str.emplace_back(str.substr(pos1, pos2 - pos1));
	pos1 = str.find_first_not_of(delimiter, pos2);
	pos2 = str.find_first_of(delimiter, pos1);
  }
  return split_str;
}