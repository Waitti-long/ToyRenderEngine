#pragma once

#include "GL/glew.h"
#include "glm/glm.hpp"

class ShadowMap {
 public:
  GLuint shadow_buffer;
  GLuint shadow_texture;
  glm::mat4 light_view_matrix;
  glm::mat4 light_project_matrix;
};
