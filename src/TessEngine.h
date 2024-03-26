#pragma once

#include "Engine.h"

class TessEngine : public Engine {
 public:
  void Init(GLFWwindow *window) override {
    rendering_program_.Create()
        .VertexShader("../glsl/tess/vert.glsl")
        .FragmentShader("../glsl/tess/frag.glsl")
        .TessCtrlShader("../glsl/tess/tessC.glsl")
        .TessEvalShader("../glsl/tess/tessE.glsl")
        .Link();

    SetUpMat();

    cameraX = 0.5f; cameraY = -0.5f; cameraZ = 2.0f;
  }

  float toRadians(float degrees) {
    return (degrees * 2.0f * 3.14159f) / 360.0f;
  }

  void Draw(double dt) override {
    UpdateVPMat();

    auto mMat = glm::mat4(1.0f);
    mMat = glm::rotate(mMat, toRadians(35.0f), glm::vec3(1.0f, 0.0f, 0.0f));

    glUseProgram(rendering_program_.program());
    UpdateUniformMat4fv("mvp_matrix", pMat * vMat * mMat);

    glFrontFace(GL_CCW);

    glPatchParameteri(GL_PATCH_VERTICES, 1);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glDrawArrays(GL_PATCHES, 0, 1);
  }
};
