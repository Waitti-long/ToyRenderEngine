#define TINYOBJLOADER_IMPLEMENTATION

#include "RenderingModel.h"

#include <iostream>
#include <utility>

#include "tiny_obj_loader.h"

namespace engine {

void Mesh::Setup() {
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);

  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0],
               GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void*)offsetof(Vertex, texture_coord));
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void*)offsetof(Vertex, normal));
  glEnableVertexAttribArray(3);
  glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void*)offsetof(Vertex, color));

  glBindVertexArray(0);
}

RenderingModel::RenderingModel(std::string path)
    : model_path_(std::move(path)) {}

RenderingModel RenderingModel::LoadModel(const std::string& path) {
  auto model = RenderingModel(path);
  model.LoadModel();
  return model;
}

void RenderingModel::LoadModel() {
  std::vector<Vertex> vertices;
  std::vector<GLuint> indices;
  std::vector<Texture> textures;

  tinyobj::attrib_t attrib;
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;

  std::string warn, err;

  bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err,
                              model_path_.c_str());

  if (!warn.empty()) {
    std::cout << warn << std::endl;
  }

  if (!err.empty()) {
    std::cout << err << std::endl;
  }

  if (!ret) {
    exit(-1);
  }

  for (auto& shape : shapes) {
    size_t index_offset = 0;
    for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++) {
      auto fv = size_t(shape.mesh.num_face_vertices[f]);

      for (size_t v = 0; v < fv; v++) {
        Vertex vertex{};

        auto idx = shape.mesh.indices[index_offset + v];
        vertex.position.x = attrib.vertices[3 * size_t(idx.vertex_index) + 0];
        vertex.position.y = attrib.vertices[3 * size_t(idx.vertex_index) + 1];
        vertex.position.z = attrib.vertices[3 * size_t(idx.vertex_index) + 2];

        if (idx.normal_index >= 0) {
          vertex.normal.x = attrib.normals[3 * size_t(idx.normal_index) + 0];
          vertex.normal.y = attrib.normals[3 * size_t(idx.normal_index) + 1];
          vertex.normal.z = attrib.normals[3 * size_t(idx.normal_index) + 2];
        }

        if (idx.texcoord_index >= 0) {
          vertex.texture_coord.x =
              attrib.texcoords[2 * size_t(idx.texcoord_index) + 0];
          vertex.texture_coord.y =
              attrib.texcoords[2 * size_t(idx.texcoord_index) + 1];
        }

        vertex.color.x = attrib.colors[3 * size_t(idx.vertex_index) + 0];
        vertex.color.y = attrib.colors[3 * size_t(idx.vertex_index) + 0];
        vertex.color.z = attrib.colors[3 * size_t(idx.vertex_index) + 0];

        auto material_id = shape.mesh.material_ids[f];
        if (material_id >= 0) {
          auto& material = materials[material_id];
          vertex.color.x = material.diffuse[0];
          vertex.color.y = material.diffuse[1];
          vertex.color.z = material.diffuse[2];
        }

        vertices.push_back(vertex);
      }
      index_offset += fv;
    }
  }

  meshes.emplace_back(vertices, textures);
}

void RenderingModel::ActivateTextureMipMap(GLuint texture) {
  glBindTexture(GL_TEXTURE_2D, texture);
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

void RenderingModel::Draw() {
  for (auto& mesh : meshes) {
    glBindVertexArray(mesh.VAO);
    glBindVertexArray(mesh.VAO);
    glDrawArrays(GL_TRIANGLES, 0, mesh.vertices.size() * 3);
    glBindVertexArray(0);
  }
}

}  // namespace engine