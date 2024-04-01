#pragma once

#include <string>
#include <utility>

#include "GL/glew.h"
#include "src/ImportedModel.h"
#include "src/files.h"

namespace engine {
class RenderingModel {
 public:
  static RenderingModel FromPath(const std::string& model_path,
                                 const std::string& vert_path,
                                 const std::string& frag_path);

  static RenderingModel FromString(const std::string& model_path,
                                   const std::string& vert_content,
                                   const std::string& frag_content);

  static RenderingModel Universe(const std::string& simple_model_path,
                                 const std::string& simple_texture_path);

  RenderingModel& Link();

  GLuint program() const { return program_; }

  GLuint texture() const { return texture_; }

  RenderingModel& LoadTexture(const std::string& path);

  void ActiveTexture();

  void AttachShader(const std::string& content, int type) const;

  glm::mat4 model_mat = glm::mat4(1.0f);
  std::vector<float> pvalues;
  std::vector<float> tvalues;
  std::vector<float> nvalues;
  int num_vertices;

 private:
  RenderingModel(const std::string& model_path, std::string vert,
                 std::string frag);

  ImportedModel model_;
  GLuint program_;
  GLuint texture_;
  std::string model_path_;
  std::string vert_content_;
  std::string frag_content_;
};
}  // namespace engine
