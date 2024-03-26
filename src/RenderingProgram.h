#pragma once

#include <iostream>

#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "files.h"

class RenderingProgram {
 public:
  explicit RenderingProgram() = default;

  RenderingProgram &Create() {
    program_ = glCreateProgram();
    return *this;
  }

  RenderingProgram &VertexShader(const std::string &file_path) {
    auto content = Files::ReadShaderFile(file_path.c_str());
    const char *c_str = content.c_str();
    vertex_shader_ = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader_, 1, &c_str, nullptr);
    glCompileShader(vertex_shader_);
    glAttachShader(program_, vertex_shader_);
    return *this;
  }

  RenderingProgram &FragmentShader(const std::string &file_path) {
    auto content = Files::ReadShaderFile(file_path.c_str());
    const char *c_str = content.c_str();
    fragment_shader_ = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader_, 1, &c_str, nullptr);
    glCompileShader(fragment_shader_);
    glAttachShader(program_, fragment_shader_);
    return *this;
  }

  RenderingProgram &TessCtrlShader(const std::string &file_path) {
    auto content = Files::ReadShaderFile(file_path.c_str());
    const char *c_str = content.c_str();
    tess_ctrl_shader_ = glCreateShader(GL_TESS_CONTROL_SHADER);
    glShaderSource(tess_ctrl_shader_, 1, &c_str, nullptr);
    glCompileShader(tess_ctrl_shader_);
    glAttachShader(program_, tess_ctrl_shader_);
    return *this;
  }

  RenderingProgram &TessEvalShader(const std::string &file_path) {
    auto content = Files::ReadShaderFile(file_path.c_str());
    const char *c_str = content.c_str();
    tess_eval_shader_ = glCreateShader(GL_TESS_EVALUATION_SHADER);
    glShaderSource(tess_eval_shader_, 1, &c_str, nullptr);
    glCompileShader(tess_eval_shader_);
    glAttachShader(program_, tess_eval_shader_);
    return *this;
  }

  GLuint Link() {
    glLinkProgram(program_);
    return program_;
  }

  GLuint program() const { return program_; }

  GLuint vertex_shader() const { return vertex_shader_; }

  GLuint fragment_shader() const { return fragment_shader_; }

 private:
  GLuint program_;
  GLuint vertex_shader_;
  GLuint fragment_shader_;
  GLuint tess_ctrl_shader_;
  GLuint tess_eval_shader_;
};
