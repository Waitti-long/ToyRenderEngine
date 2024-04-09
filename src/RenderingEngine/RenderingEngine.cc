#include "RenderingEngine.h"

namespace engine {

void APIENTRY glDebugOutput(GLenum source, GLenum type, GLuint id,
                            GLenum severity, GLsizei length,
                            const GLchar *message, const void *userParam) {
  std::cout << "Debug : " << message << std::endl;
}

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
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
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

  {
    // debug
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(glDebugOutput, NULL);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL,
                          GL_TRUE);
  }
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

void RenderingEngine::UpdateUniform2fv(GLuint program, const std::string &name,
                                       float *value) {
  GLuint loc = glGetUniformLocation(program, name.c_str());
  glProgramUniform2fv(program, loc, 1, value);
}

void RenderingEngine::UpdateUniform1fv(GLuint program, const std::string &name,
                                       float value) {
  GLuint loc = glGetUniformLocation(program, name.c_str());
  glProgramUniform1fv(program, loc, 1, &value);
}

void RenderingEngine::UpdateUniformArrayVec3fv(
    GLuint program, const std::string &name,
    const std::vector<glm::vec3> &value) {
  GLuint loc = glGetUniformLocation(program, name.c_str());
  glProgramUniform3fv(program, loc, value.size(), (float *)&value[0]);
}

GLuint RenderingEngine::UseProgram(RenderingEngine::ProgramType type) {
  auto create_link_program = [](GLuint &program, const std::string &vertex,
                                const std::string &frag) {
    program = glCreateProgram();
    AttachShader(program, Files::ReadShaderFile(vertex.c_str()),
                 GL_VERTEX_SHADER);
    AttachShader(program, Files::ReadShaderFile(frag.c_str()),
                 GL_FRAGMENT_SHADER);
    glLinkProgram(program);
  };

  int index = (int)type;
  auto &program = programs_[index];
  if (program == 0) {
    switch (type) {
      case ProgramType::DEFAULT:
        create_link_program(program, "../glsl/universe/vertex.glsl",
                            "../glsl/universe/fragment.glsl");
        break;
      case ProgramType::SHADOW_MAP:
        create_link_program(program, "../glsl/sm_vert.glsl",
                            "../glsl/sm_frag.glsl");
        break;
      case ProgramType::G_BUFFER:
        create_link_program(program, "../glsl/g_vert.glsl",
                            "../glsl/g_frag.glsl");
        break;
      case ProgramType::SSAO:
        create_link_program(program, "../glsl/ssao_vert.glsl",
                            "../glsl/ssao_frag.glsl");
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
