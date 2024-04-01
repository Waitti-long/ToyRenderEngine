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

  void UpdateUniformMat4fv(GLuint program, const std::string& name,
                           const glm::mat4& value);

  void UpdateUniform4fv(GLuint program, const std::string& name,
                        float value[4]);

  void UpdateUniform3fv(GLuint program, const std::string& name,
                        float value[3]);

  static constexpr int NUM_VAO = 1;
  static constexpr int NUM_VBO = 3;

 protected:
  RenderingStore store_{};
  RenderingSettings settings_{};
  GLFWwindow* window_ = nullptr;
  double last_time_{};

  GLuint vao[NUM_VAO]{};
  GLuint vbo[NUM_VBO]{};
};

}  // namespace engine
