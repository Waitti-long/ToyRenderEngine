#pragma once

#include <vector>

#include "Lights.h"
#include "RenderingModel.h"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

namespace engine {

class ShadowMap {
 public:
  GLuint shadow_buffer;
  GLuint shadow_texture;
  glm::mat4 light_view_matrix;
  glm::mat4 light_project_matrix;
};

class GBuffer {
 public:
  GLuint g_buffer;
  GLuint g_position;
  GLuint g_normal;
  GLuint g_color;
  GLuint g_depth;
};

class SSAOData {
 public:
  std::vector<glm::vec3> samples;
  GLuint noise_texture;
  GLuint fbo;
  GLuint color_buffer;
};

class RenderingStore {
 public:
  void OnWindowSizeChanged(int width, int height);

  void RotateCamera(glm::vec3 value);

  void MoveCamera(glm::vec3 value);

  float z_near = 0.1f;
  float z_far = 1000.0f;
  glm::mat4 perspective_matrix;
  glm::vec3 camera_position, camera_rotate;
  glm::mat4 view_matrix;
  float aspect;
  std::vector<RenderingModel> models;
  std::vector<SpotLight> spot_lights;
  ShadowMap shadow_map;
  GBuffer g_buffer;
  SSAOData ssao;
};

}  // namespace engine