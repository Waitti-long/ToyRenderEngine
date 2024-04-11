#pragma once

#include "RenderingEngine.h"

namespace engine {

class RenderingEngineDefault : public RenderingEngine {
 public:
  void Init() override;

  void Draw(double dt) override;

  void HandleInput(GLFWwindow* window, double dt) override;

  void DrawModelWithProgramDefault(RenderingModel& model, GLuint program);

  void DrawModelsWithProgramShadowMap(GLuint program);

  void DrawModelsWithProgramGBuffer(GLuint program);

  void DrawModelsWidthProgramSSAO(GLuint program);

  void TestComputeShader(GLuint program);

 private:
  void GenerateSSAOSamplesAndNoise();
};

}  // namespace engine
