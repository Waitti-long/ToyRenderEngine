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
    auto model = RenderingModel::LoadModel("../assets/cornell-box.obj");
    model.model_mat = glm::translate(model.model_mat, glm::vec3(0.0, -1, 0.0));
    store_.models.push_back(model);
  }

  {
    auto spot_light = SpotLight();
    spot_light.position = glm::vec3(1.1003, 2.76733, 0.165055);
    spot_light.color = glm::vec4(1, 1, 1, 1);

    store_.spot_lights.push_back(spot_light);
  }

  settings_.shadow = RenderingSettings::Shadow::SHADOW_MAP;
  settings_.ao = RenderingSettings::AO::SSAO;
}

void RenderingEngineDefault::Draw(double dt) {
  if (settings_.shadow == RenderingSettings::Shadow::SHADOW_MAP) {
    DrawModelsWidthProgramShadowMap(UseProgram(ProgramType::SHADOW_MAP));
  }

  for (RenderingModel& model : store_.models) {
    DrawModelWidthProgramDefault(model, UseProgram(ProgramType::DEFAULT));
  }
}

void RenderingEngineDefault::HandleInput(GLFWwindow* window, double dt) {
  auto move_light = [this](double x, double y, double z) {
    if (!store_.spot_lights.empty()) {
      auto& light = store_.spot_lights[0];
      light.position += glm::vec3(x, y, z);
      std::cout << "light: " << light.position.x << ", " << light.position.y
                << ", " << light.position.z << std::endl;
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

void RenderingEngineDefault::DrawModelWidthProgramDefault(RenderingModel& model,
                                                          GLuint program) {
  glm::mat4 mv_matrix = store_.view_matrix * model.model_mat;
  glm::mat4 inv_tr_matrix = glm::transpose(glm::inverse(mv_matrix));
  UpdateUniformMat4fv(program, "mv_matrix", mv_matrix);
  UpdateUniformMat4fv(program, "proj_matrix", store_.perspective_matrix);
  UpdateUniformMat4fv(program, "norm_matrix", inv_tr_matrix);

  if (settings_.shadow == RenderingSettings::Shadow::SHADOW_MAP) {
    static glm::mat4 b =
        glm::mat4(0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f,
                  0.5f, 0.0f, 0.5f, 0.5f, 0.5f, 1.0f);
    glm::mat4 shadow_mvp = b * store_.shadow_map.light_project_matrix *
                           store_.shadow_map.light_view_matrix *
                           model.model_mat;
    UpdateUniformMat4fv(program, "shadow_mvp", shadow_mvp);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, store_.shadow_map.shadow_texture);
    UpdateUniform1fv(program, "enable_shadow_map", 1.0f);
  } else {
    UpdateUniform1fv(program, "enable_shadow_map", 0.0f);
  }

  if (!store_.spot_lights.empty()) {
    auto& light = store_.spot_lights[0];
    auto light_pos =
        glm::vec3(store_.view_matrix * glm::vec4(light.position, 1.0));
    float light_arr[3] = {light_pos.x, light_pos.y, light_pos.z};
    float color_arr[4] = {light.color.r, light.color.g, light.color.b,
                          light.color.a};
    UpdateUniform3fv(program, "spot_light.position", light_arr);
    UpdateUniform4fv(program, "spot_light.color", color_arr);
  }

  glEnable(GL_CULL_FACE);
  glFrontFace(GL_CCW);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);

  model.Draw(this, program);

  Logger::PrintProgramLog(program);
  glUseProgram(0);
}

void RenderingEngineDefault::DrawModelsWidthProgramShadowMap(GLuint program) {
  int width, height;
  GLuint& shadow_buffer = store_.shadow_map.shadow_buffer;
  GLuint& shadow_texture = store_.shadow_map.shadow_texture;

  if (shadow_buffer == 0 && shadow_texture == 0) {
    // 创建自定义帧缓冲区
    glGenFramebuffers(1, &shadow_buffer);
    // 创建阴影纹理
    glGenTextures(1, &shadow_texture);

    glfwGetFramebufferSize(window_, &width, &height);
    glBindTexture(GL_TEXTURE_2D, shadow_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height, 0,
                 GL_DEPTH_COMPONENT, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE,
                    GL_COMPARE_REF_TO_TEXTURE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
  }

  assert(!store_.spot_lights.empty());
  auto light = store_.spot_lights[0];
  store_.shadow_map.light_view_matrix =
      glm::lookAt(light.position, glm::vec3(), glm::vec3(0, 1, 0));
  store_.shadow_map.light_project_matrix = store_.perspective_matrix;

  // 绑定阴影纹理
  glBindFramebuffer(GL_FRAMEBUFFER, shadow_buffer);
  glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadow_texture, 0);

  glClear(GL_DEPTH_BUFFER_BIT);
  glEnable(GL_CULL_FACE);

  // 关闭绘制颜色
  glDrawBuffer(GL_NONE);

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);

  glEnable(GL_POLYGON_OFFSET_FILL);  // 减少伪影
  glPolygonOffset(2.0f, 4.0f);

  for (RenderingModel& model : store_.models) {
    glm::mat4 mv_matrix = store_.shadow_map.light_view_matrix * model.model_mat;
    UpdateUniformMat4fv(program, "mv_matrix", mv_matrix);
    UpdateUniformMat4fv(program, "proj_matrix",
                        store_.shadow_map.light_project_matrix);

    model.Draw(this, program);
  }

  glDisable(GL_POLYGON_OFFSET_FILL);

  // 使用显示缓冲区, 重新开启绘制
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glDrawBuffer(GL_FRONT);

  Logger::PrintProgramLog(program);
  glUseProgram(0);
}

}  // namespace engine