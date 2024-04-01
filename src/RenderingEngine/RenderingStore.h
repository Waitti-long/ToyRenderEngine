#pragma once

#include <vector>

#include "RenderingModel.h"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

namespace engine {

class RenderingStore {
 public:
  void OnWindowSizeChanged(int width, int height);

  void RotateCamera(glm::vec3 value);

  void MoveCamera(glm::vec3 value);

  glm::mat4 perspective_matrix;
  glm::vec3 camera_position, camera_rotate;
  glm::mat4 view_matrix;
  float aspect;
  std::vector<RenderingModel> models;
};

}  // namespace engine