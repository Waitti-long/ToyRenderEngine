#pragma once

#include <memory>
#include <mutex>

#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "RenderingSettings.h"
#include "RenderingStore.h"
#include "glm/glm.hpp"
#include "src/logger.h"

namespace engine {

class RenderingEngine {
 public:
  enum class ProgramType : uint32_t {
    DEFAULT = 0,
    SHADOW_MAP = 1,
    G_BUFFER = 2,
    SSAO = 3,
    COUNT = 4
  };

  virtual void Init();

  virtual void HandleInput(GLFWwindow* window, double dt) {}

  virtual void Draw(double dt) {}

  virtual void WindowChanged(GLFWwindow* window, int new_width, int new_height);

  void Start();

  template <class T>
  static T& Singleton() {
    static std::once_flag flag;
    static std::shared_ptr<T> rendering_engine;
    std::call_once(flag, [&]() { rendering_engine.reset(new T()); });
    return *rendering_engine;
  }

  void Clear();

  void UpdateUniform1iv(GLuint program, const std::string& name, int value);

  void UpdateUniformMat4fv(GLuint program, const std::string& name,
                           const glm::mat4& value);

  void UpdateUniform4fv(GLuint program, const std::string& name,
                        float value[4]);

  void UpdateUniform3fv(GLuint program, const std::string& name,
                        float value[3]);

  void UpdateUniform2fv(GLuint program, const std::string& name, float* value);

  void UpdateUniform1fv(GLuint program, const std::string& name, float value);

  void UpdateUniformArrayVec3fv(GLuint program, const std::string& name,
                                const std::vector<glm::vec3>&);

  GLuint UseProgram(ProgramType type);

  static void AttachShader(GLuint program, const std::string& content,
                           int type);

  static constexpr int NUM_VAO = 1;
  static constexpr int NUM_VBO = 3;

 protected:
  RenderingStore store_{};
  RenderingSettings settings_{};
  GLFWwindow* window_ = nullptr;
  double last_time_{};

  GLuint vao[NUM_VAO]{};
  GLuint vbo[NUM_VBO]{};

  GLuint programs_[int(ProgramType::COUNT)]{};
};

}  // namespace engine
