#include "RenderingEngineDefault.h"

namespace engine {

void window_size_changed_callback(GLFWwindow* window, int width, int height) {
  RenderingEngine::Singleton<RenderingEngineDefault>().WindowChanged(
      window, width, height);
}

void RenderingEngineDefault::Init() {
  RenderingEngine::Init();

  glfwSetWindowSizeCallback(window_, window_size_changed_callback);

  {
    int width, height;
    glfwGetFramebufferSize(window_, &width, &height);
    store_.OnWindowSizeChanged(width, height);
    store_.MoveCamera(glm::vec3());
  }

  { store_.MoveCamera(glm::vec3(0, 0, 4)); }

  {
    // DOWN
    auto face = RenderingModel::Universe("face.obj", "brown.jpg");
    face.model_mat = glm::scale(glm::mat4(1.0f), glm::vec3(1.5, 1.5, 1.5)) *
                     glm::rotate(glm::mat4(1.0f), 1.0f, glm::vec3(1, 0, 0)) *
                     face.model_mat;

    store_.models.push_back(face);
  }

  {
    // BACK
    auto face = RenderingModel::Universe("face.obj", "brown.jpg");
    face.model_mat = glm::scale(glm::mat4(1.0f), glm::vec3(1.5, 1.5, 1.5)) *
                     glm::rotate(glm::mat4(1.0f), 2.0f, glm::vec3(1, 0, 0)) *
                     glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -1)) *
                     face.model_mat;

    store_.models.push_back(face);
  }

  {
    // LEFT
    auto face = RenderingModel::Universe("face.obj", "brown.jpg");
    face.model_mat = glm::scale(glm::mat4(1.0f), glm::vec3(1.5, 1.5, 1.5)) *
                     glm::rotate(glm::mat4(1.0f), 2.0f, glm::vec3(1, 0, 0)) *
                     glm::translate(glm::mat4(1.0f), glm::vec3(-1, 0, 0)) *
                     glm::rotate(glm::mat4(1.0f), 2.0f, glm::vec3(0, 1, 0)) *
                     face.model_mat;

    store_.models.push_back(face);
  }

  {
    // RIGHT
    auto face = RenderingModel::Universe("face.obj", "brown.jpg");
    face.model_mat = glm::scale(glm::mat4(1.0f), glm::vec3(1.5, 1.5, 1.5)) *
                     glm::rotate(glm::mat4(1.0f), 2.0f, glm::vec3(1, 0, 0)) *
                     glm::translate(glm::mat4(1.0f), glm::vec3(1, 0, 0)) *
                     glm::rotate(glm::mat4(1.0f), 2.0f, glm::vec3(0, -1, 0)) *
                     face.model_mat;

    store_.models.push_back(face);
  }

  {
    auto cube = RenderingModel::Universe("cube.obj", "red.jpg");
    cube.model_mat = glm::scale(glm::mat4(1.0f), glm::vec3(0.5, 0.5, 0.5)) *
                     glm::rotate(glm::mat4(1.0f), 1.0f, glm::vec3(1, 0, 0)) *
                     glm::translate(glm::mat4(1.0f), glm::vec3(-2, 1, 0)) *
                     cube.model_mat;

    store_.models.push_back(cube);
  }

  {
    auto sphere = RenderingModel::Universe("sphere.obj", "green.jpg");
    sphere.model_mat = glm::scale(glm::mat4(1.0f), glm::vec3(0.5, 0.5, 0.5)) *
                       glm::rotate(glm::mat4(1.0f), 1.0f, glm::vec3(1, 0, 0)) *
                       glm::translate(glm::mat4(1.0f), glm::vec3(2, 1, 0)) *
                       sphere.model_mat;

    store_.models.push_back(sphere);
  }
}

void RenderingEngineDefault::Draw(double dt) { DrawModels(); }

void RenderingEngineDefault::DrawModels() {
  for (RenderingModel& model : store_.models) {
    DrawModel(model);
  }
}

void RenderingEngineDefault::DrawModel(RenderingModel& model) {
  glUseProgram(model.program());

  glm::mat4 mv_matrix = store_.view_matrix * model.model_mat;
  glm::mat4 inv_tr_matrix = glm::transpose(glm::inverse(mv_matrix));
  UpdateUniformMat4fv(model.program(), "mv_matrix", mv_matrix);
  UpdateUniformMat4fv(model.program(), "proj_matrix",
                      store_.perspective_matrix);
  UpdateUniformMat4fv(model.program(), "norm_matrix", inv_tr_matrix);

  glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(1);

  glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(2);

  {
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, model.pvalues.size() * 4, &model.pvalues[0],
                 GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, model.tvalues.size() * 4, &model.tvalues[0],
                 GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
    glBufferData(GL_ARRAY_BUFFER, model.nvalues.size() * 4, &model.nvalues[0],
                 GL_STATIC_DRAW);
  }

  model.ActiveTexture();

  glEnable(GL_CULL_FACE);
  glFrontFace(GL_CCW);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);
  glDrawArrays(GL_TRIANGLES, 0, model.num_vertices);
  Logger::PrintProgramLog(model.program());

  glUseProgram(0);
}

}  // namespace engine