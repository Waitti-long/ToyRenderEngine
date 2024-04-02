#pragma once

#include "RenderingEngine.h"

namespace engine {

class RenderingEngineDefault : public RenderingEngine {
 public:
  void Init() override;

  void Draw(double dt) override;

  void HandleInput(GLFWwindow* window, double dt) override;

  void DrawModelWidthProgramDefault(RenderingModel& model, GLuint program);

  void DrawModelsWidthProgramShadowMap(GLuint program);
};

}  // namespace engine
