#pragma once

#include <string>
#include <utility>
#include <vector>

#include "GL/glew.h"
#include "glm/glm.hpp"
#include "src/files.h"

namespace engine {

class RenderingEngine;

class Vertex {
 public:
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec2 texture_coord;
  glm::vec3 color;
};

class Texture {
 public:
  GLuint id;
  std::string type;
};

class Mesh {
 public:
  Mesh(std::vector<Vertex> vertices, std::vector<Texture> textures)
      : vertices(std::move(vertices)), textures(std::move(textures)) {
    Setup();
  }

  std::vector<Vertex> vertices;
  std::vector<Texture> textures;
  GLuint VAO, VBO;

 private:
  void Setup();
};

class RenderingModel {
 public:
  static RenderingModel LoadModel(const std::string& model_path);

  void LoadModel();

  static void ActivateTextureMipMap(GLuint texture);

  void Draw();

  std::vector<Mesh> meshes;
  glm::mat4 model_mat = glm::mat4(1.0f);

 private:
  explicit RenderingModel(std::string  model_path);

  std::string model_path_;
};
}  // namespace engine
