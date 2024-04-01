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

  {
    auto spot_light = SpotLight();
    spot_light.position = glm::vec3(0.2012, -2.01696, 0.814765);
    spot_light.color = glm::vec4(1, 1, 1, 1);

    store_.spot_lights.push_back(spot_light);
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

  if (!store_.spot_lights.empty()) {
    auto& light = store_.spot_lights[0];
    auto light_pos =
        glm::vec3(store_.view_matrix * glm::vec4(light.position, 1.0));
    float light_arr[3] = {light_pos.x, light_pos.y, light_pos.z};
    float color_arr[4] = {light.color.r, light.color.g, light.color.b,
                          light.color.a};
    UpdateUniform3fv(model.program(), "spot_light.position", light_arr);
    UpdateUniform4fv(model.program(), "spot_light.color", color_arr);
  }

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

void RenderingEngineDefault::HandleInput(GLFWwindow* window, double dt) {
  auto move_light = [this](double x, double y, double z) {
    if (!store_.spot_lights.empty()) {
      auto& light = store_.spot_lights[0];
      light.position += glm::vec3(x, y, z);
      std::cout << light.position.x << ", " << light.position.y << ", "
                << light.position.z << std::endl;
    }
  };

  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
    move_light(dt, 0, 0);
  } else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
    move_light(-dt, 0, 0);
  } else if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
    move_light(0, dt, 0);
  } else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
    move_light(0, -dt, 0);
  } else if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
    move_light(0, 0, -dt);
  } else if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
    move_light(0, 0, dt);
  }
}

}  // namespace engine