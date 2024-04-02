#include "RenderingModel.h"

#include <iostream>
#include <utility>

#include "RenderingEngine.h"
#include "third_party/SOIL2/src/SOIL2/SOIL2.h"

namespace engine {

RenderingModel::RenderingModel(const std::string& model_path, std::string vert,
                               std::string frag)
    : model_(model_path.c_str()),
      vert_content_(std::move(vert)),
      frag_content_(std::move(frag)) {}

RenderingModel& RenderingModel::Link() {
  if (!vert_content_.empty() && !frag_content_.empty()) {
    program_ = glCreateProgram();
    RenderingEngine::AttachShader(program_, vert_content_, GL_VERTEX_SHADER);
    RenderingEngine::AttachShader(program_, frag_content_, GL_FRAGMENT_SHADER);
    glLinkProgram(program_);
  }

  {
    auto vert = model_.getVertices();
    auto tex = model_.getTextureCoords();
    auto norm = model_.getNormals();
    int numObjVertices = model_.getNumVertices();

    pvalues.clear();
    tvalues.clear();
    nvalues.clear();

    for (int i = 0; i < numObjVertices; i++) {
      pvalues.push_back(vert[i].x);
      pvalues.push_back(vert[i].y);
      pvalues.push_back(vert[i].z);
      tvalues.push_back(tex[i].s);
      tvalues.push_back(tex[i].t);
      nvalues.push_back(norm[i].x);
      nvalues.push_back(norm[i].y);
      nvalues.push_back(norm[i].z);
    }

    num_vertices = numObjVertices;
  }

  return *this;
}

RenderingModel RenderingModel::FromPath(const std::string& model_path,
                                        const std::string& vert_path,
                                        const std::string& frag_path) {
  auto vert = vert_path.empty() ? "" : Files::ReadShaderFile(vert_path.c_str());
  auto frag = frag_path.empty() ? "" : Files::ReadShaderFile(frag_path.c_str());
  return RenderingModel{model_path, vert, frag};
}

RenderingModel RenderingModel::FromString(const std::string& model_path,
                                          const std::string& vert_content,
                                          const std::string& frag_content) {
  return RenderingModel{model_path, vert_content, frag_content};
}

RenderingModel RenderingModel::Universe(
    const std::string& simple_model_path,
    const std::string& simple_texture_path) {
  return FromPath("../assets/" + simple_model_path, "", "")
      .LoadTexture("../assets/" + simple_texture_path)
      .Link();
}

RenderingModel& RenderingModel::LoadTexture(const std::string& path) {
  texture_ = SOIL_load_OGL_texture(path.c_str(), SOIL_LOAD_AUTO,
                                   SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
  if (texture_ == 0) {
    std::cout << "could not find texture file: " << path << std::endl;
    exit(EXIT_FAILURE);
  }
  return *this;
}

void RenderingModel::ActiveTexture() {
  if (texture_ != 0) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_);
    // mipmap
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR_MIPMAP_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);
    // 各向异性过滤
    if (glewIsSupported("GL_EXT_texture_filter_anisotropic")) {
      GLfloat anisoSettings = 0.0f;
      glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &anisoSettings);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT,
                      anisoSettings);
    }
  }
}

}  // namespace engine