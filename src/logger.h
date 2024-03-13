#pragma once

#include <GL/glew.h>

#include <iostream>

class Logger {
 public:
  static void PrintShaderLog(GLuint shader) {
    int len = 0;
    int ch_writtn = 0;
    char *log;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
    if (len > 0) {
      log = (char *)malloc(len);
      glGetShaderInfoLog(shader, len, &ch_writtn, log);
      std::cout << "Shader Info Log: " << log << std::endl;
      free(log);
    }
  }

  static void PrintProgramLog(int prog) {
    int len = 0;
    int ch_writtn = 0;
    char *log;
    glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &len);
    if (len > 0) {
      log = (char *)malloc(len);
      glGetProgramInfoLog(prog, len, &ch_writtn, log);
      std::cout << "Program Info Log: " << log << std::endl;
      free(log);
    }
  }

  static bool CheckOpenGLError() {
    bool foundError = false;
    int glErr = glGetError();
    while (glErr != GL_NO_ERROR) {
      std::cout << "glError: " << glErr << std::endl;
      foundError = true;
      glErr = glGetError();
    }
    return foundError;
  }
};
