#include "RenderingStore.h"

namespace engine {

void RenderingStore::OnWindowSizeChanged(int width, int height) {
  aspect = (float)height / (float)width;
  perspective_matrix = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);
}

void RenderingStore::RotateCamera(glm::vec3 value) {
  //  camera_rotate = camera_rotate + value;
}

void RenderingStore::MoveCamera(glm::vec3 value) {
  camera_position = camera_position + value;
  view_matrix = glm::translate(glm::mat4(1.0f), -camera_position);
}

}  // namespace engine