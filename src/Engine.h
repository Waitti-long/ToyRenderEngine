#pragma once

#include <iostream>

#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "ImportedModel.h"
#include "RenderingProgram.h"
#include "Sphere.h"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "logger.h"
#include "material.h"
#include "third_party/SOIL2/src/SOIL2/SOIL2.h"

#define numVAOs 1
#define numVBOs 3

class Engine {
 public:
  explicit Engine() = default;

  void Init(GLFWwindow *window) {
    rendering_program_.Create()
        .VertexShader("../glsl/vertex.glsl")
        .FragmentShader("../glsl/fragment.glsl")
        .Link();

    model = ImportedModel("../assets/shuttle.obj");
    texture = LoadTexture("../assets/spstob_1.jpg");

    SetUpMat();
    SetUpVertices();
  }

  void SetUpMat() {
    cameraX = 0.0f;
    cameraY = 0.0f;
    cameraZ = 2.0f;

    offsetX = 0.0f;
    offsetY = 0.0f;
    offsetZ = 0.0f;

    thea = 0.0f;
  }

  void SetUpVertices() {
    auto vert = model.getVertices();
    auto tex = model.getTextureCoords();
    auto norm = model.getNormals();
    int numObjVertices = model.getNumVertices();

    std::vector<float> pvalues;
    std::vector<float> tvalues;
    std::vector<float> nvalues;

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

    glGenVertexArrays(1, vao);
    glBindVertexArray(vao[0]);
    glGenBuffers(numVBOs, vbo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, pvalues.size() * 4, &pvalues[0],
                 GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, tvalues.size() * 4, &tvalues[0],
                 GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
    glBufferData(GL_ARRAY_BUFFER, nvalues.size() * 4, &nvalues[0],
                 GL_STATIC_DRAW);
  }

  void Draw(double dt) {
    glUseProgram(rendering_program_.program());

    UpdateMat(dt);
    UpdateUniformMat4fv("mv_matrix", mvMat);
    UpdateUniformMat4fv("proj_matrix", pMat);
    UpdateUniformMat4fv("norm_matrix", invTrMat);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2);

    glActiveTexture(GL_TEXTURE0);
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

    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glDrawArrays(GL_TRIANGLES, 0, model.getNumVertices());
  }

  void UpdateMat(double dt) {
    glfwGetFramebufferSize(window_, &width, &height);
    aspect = (float)width / (float)height;
    pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);

    vMat = glm::translate(glm::mat4(1.0f),
                          glm::vec3(-cameraX, -cameraY, -cameraZ));
    thea += dt;
    auto rMat =
        glm::rotate(glm::mat4(1.0f), thea, glm::normalize(glm::vec3(1, 1, 0)));
    mMat =
        glm::translate(glm::mat4(1.0f), glm::vec3(offsetX, offsetY, offsetZ));
    mMat *= rMat;
    mvMat = vMat * mMat;

    currentLightPos =
        glm::vec3(initialLightLoc.x, initialLightLoc.y, initialLightLoc.z);
    installLights(vMat);

    invTrMat = glm::transpose(glm::inverse(mvMat));
  }

  void installLights(glm::mat4 vMatrix) {
    lightPosV = glm::vec3(vMatrix * glm::vec4(currentLightPos, 1.0f));
    lightPos[0] = lightPosV.x;
    lightPos[1] = lightPosV.y;
    lightPos[2] = lightPosV.z;

    UpdateUniform4fv("globalAmbient", globalAmbient);
    UpdateUniform4fv("light.ambient", lightAmbient);
    UpdateUniform4fv("light.diffuse", lightDiffuse);
    UpdateUniform4fv("light.specular", lightSpecular);
    UpdateUniform3fv("light.position", lightPos);
    UpdateUniform4fv("material.ambient", matAmb);
    UpdateUniform4fv("material.diffuse", matDif);
    UpdateUniform4fv("material.specular", matSpe);
    UpdateUniform1f("material.shininess", matShi);
  }

  void UpdateUniform1f(const std::string &name, double value) {
    GLuint loc =
        glGetUniformLocation(rendering_program_.program(), name.c_str());
    glProgramUniform1f(rendering_program_.program(), loc, value);
  }

  void UpdateUniformMat4fv(const std::string &name, const glm::mat4 &value) {
    GLuint loc =
        glGetUniformLocation(rendering_program_.program(), name.c_str());
    glProgramUniformMatrix4fv(rendering_program_.program(), loc, 1, GL_FALSE,
                              glm::value_ptr(value));
  }

  void UpdateUniform4fv(const std::string &name, float value[4]) {
    GLuint loc =
        glGetUniformLocation(rendering_program_.program(), name.c_str());
    glProgramUniform4fv(rendering_program_.program(), loc, 1, value);
  }

  void UpdateUniform3fv(const std::string &name, float value[3]) {
    GLuint loc =
        glGetUniformLocation(rendering_program_.program(), name.c_str());
    glProgramUniform3fv(rendering_program_.program(), loc, 1, value);
  }

  static GLuint LoadTexture(const char *path) {
    GLuint texture_id = SOIL_load_OGL_texture(
        path, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
    if (texture_id == 0) {
      std::cout << "could not find texture file: " << path << std::endl;
      exit(EXIT_FAILURE);
    }
    return texture_id;
  }

  void Clear() {
    glClear(GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
  }

  void Start() {
    // Init glfw
    if (!glfwInit()) {
      exit(EXIT_FAILURE);
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    window_ = glfwCreateWindow(600, 600, "gllearn", nullptr, nullptr);
    glfwMakeContextCurrent(window_);

    // Init glew
    if (glewInit() != GLEW_OK) {
      exit(EXIT_FAILURE);
    }
    glfwSwapInterval(1);
    Init(window_);

    last_time_ = glfwGetTime();
    // loop
    while (!glfwWindowShouldClose(window_)) {
      double time = glfwGetTime();
      double dt = time - last_time_;
      last_time_ = time;
      Clear();
      Draw(dt);
      Logger::PrintProgramLog(rendering_program_.program());
      glfwSwapBuffers(window_);
      glfwPollEvents();
    }

    // close
    glfwDestroyWindow(window_);
    glfwTerminate();
    exit(EXIT_SUCCESS);
  }

 private:
  GLFWwindow *window_ = nullptr;
  RenderingProgram rendering_program_{};
  double last_time_{};

  GLuint vao[numVAOs]{};
  GLuint vbo[numVBOs]{};
  float cameraX, cameraY, cameraZ;
  float offsetX, offsetY, offsetZ;
  ImportedModel model;

  GLuint texture;

  int width, height;
  float aspect;
  glm::mat4 pMat, vMat, mMat, mvMat, invTrMat;
  float thea;

  glm::vec3 currentLightPos, lightPosV;  // 在模型和视觉空间中的光照位置
  float lightPos[3];                     // 光照位置的浮点数组

  // 初始化光照位置
  glm::vec3 initialLightLoc = glm::vec3(5.0f, 2.0f, 2.0f);

  // 白光
  float globalAmbient[4] = {0.7f, 0.7f, 0.7f, 1.0f};
  float lightAmbient[4] = {0.0f, 0.0f, 0.0f, 1.0f};
  float lightDiffuse[4] = {1.0f, 1.0f, 1.0f, 1.0f};
  float lightSpecular[4] = {1.0f, 1.0f, 1.0f, 1.0f};

  // 黄金
  float *matAmb = Material::goldAmbient();
  float *matDif = Material::goldDiffuse();
  float *matSpe = Material::goldSpecular();
  float matShi = Material::goldShininess();
};
