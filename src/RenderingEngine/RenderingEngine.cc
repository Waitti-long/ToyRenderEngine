#include "RenderingEngine.h"

namespace engine {

void RenderingEngine::WindowChanged(GLFWwindow *window, int new_width,
                                    int new_height) {
  glViewport(0, 0, new_width, new_height);
  store_.OnWindowSizeChanged(new_width, new_height);
}

void RenderingEngine::Start() {
  // Init glfw
  if (!glfwInit()) {
    exit(EXIT_FAILURE);
  }
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  window_ = glfwCreateWindow(1200, 1200, "RenderingEngine", nullptr, nullptr);
  glfwMakeContextCurrent(window_);

  // Init glew
  if (glewInit() != GLEW_OK) {
    exit(EXIT_FAILURE);
  }
  glfwSwapInterval(1);
  Init();

  last_time_ = glfwGetTime();
  // loop
  while (!glfwWindowShouldClose(window_)) {
    double time = glfwGetTime();
    double dt = time - last_time_;
    last_time_ = time;
    HandleInput(window_, dt);
    Clear();
    Draw(dt);
    Logger::CheckOpenGLError();
    glfwSwapBuffers(window_);
    glfwPollEvents();
  }

  // close
  glfwDestroyWindow(window_);
  glfwTerminate();
  exit(EXIT_SUCCESS);
}

void RenderingEngine::Clear() {
  glClear(GL_DEPTH_BUFFER_BIT);
  glClearColor(0.0, 0.0, 0.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT);
}

void RenderingEngine::Init() {
  glGenVertexArrays(NUM_VAO, vao);
  glBindVertexArray(vao[0]);
  glGenBuffers(NUM_VBO, vbo);
}

void RenderingEngine::UpdateUniformMat4fv(GLuint program,
                                          const std::string &name,
                                          const glm::mat4 &value) {
  GLuint loc = glGetUniformLocation(program, name.c_str());
  glProgramUniformMatrix4fv(program, loc, 1, GL_FALSE, glm::value_ptr(value));
}

void RenderingEngine::UpdateUniform4fv(GLuint program, const std::string &name,
                                       float *value) {
  GLuint loc = glGetUniformLocation(program, name.c_str());
  glProgramUniform4fv(program, loc, 1, value);
}

void RenderingEngine::UpdateUniform3fv(GLuint program, const std::string &name,
                                       float *value) {
  GLuint loc = glGetUniformLocation(program, name.c_str());
  glProgramUniform3fv(program, loc, 1, value);
}

void RenderingEngine::UpdateUniform1fv(GLuint program, const std::string &name,
                                       float value) {
  GLuint loc = glGetUniformLocation(program, name.c_str());
  glProgramUniform1fv(program, loc, 1, &value);
}

GLuint RenderingEngine::UseProgram(RenderingEngine::ProgramType type) {
  int index = (int)type;
  auto &program = programs_[index];
  if (program == 0) {
    switch (type) {
      case ProgramType::DEFAULT:
        program = glCreateProgram();
        AttachShader(program,
                     Files::ReadShaderFile("../glsl/universe/vertex.glsl"),
                     GL_VERTEX_SHADER);
        AttachShader(program,
                     Files::ReadShaderFile("../glsl/universe/fragment.glsl"),
                     GL_FRAGMENT_SHADER);
        glLinkProgram(program);
        break;
      case ProgramType::SHADOW_MAP:
        program = glCreateProgram();
        AttachShader(program, Files::ReadShaderFile("../glsl/sm_vert.glsl"),
                     GL_VERTEX_SHADER);
        AttachShader(program, Files::ReadShaderFile("../glsl/sm_frag.glsl"),
                     GL_FRAGMENT_SHADER);
        glLinkProgram(program);
        break;
      default:
        std::cout << "unsupported program type: " << index << std::endl;
        exit(EXIT_FAILURE);
    }
  }
  glUseProgram(program);
  return program;
}

void RenderingEngine::AttachShader(GLuint program, const std::string &content,
                                   int type) {
  const char *c_str = content.c_str();
  GLuint shader = glCreateShader(type);
  glShaderSource(shader, 1, &c_str, nullptr);
  glCompileShader(shader);
  glAttachShader(program, shader);
}

}  // namespace engine
