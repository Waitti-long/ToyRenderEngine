#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

class Files {
 public:
  static std::string ReadShaderFile(const char *file_path) {
    std::string content;
    std::ifstream file_stream(file_path, std::ios::in);
    std::string line;
    while (!file_stream.eof()) {
      std::getline(file_stream, line);
      content.append(line + "\n");
    }
    file_stream.close();
    return content;
  }
};