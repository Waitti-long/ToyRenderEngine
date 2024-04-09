#include "RenderingEngineDefault.h"

#include <random>

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
  settings_.pcf_kernel = 4;

  settings_.ao = RenderingSettings::AO::SSAO;

  settings_.need_gbuffer = true;
}

void RenderingEngineDefault::Draw(double dt) {
  if (settings_.need_gbuffer) {
    DrawModelsWithProgramGBuffer(UseProgram(ProgramType::G_BUFFER));
  }

  if (settings_.ao == RenderingSettings::AO::SSAO) {
    DrawModelsWidthProgramSSAO(UseProgram(ProgramType::SSAO));
  }

  if (settings_.shadow == RenderingSettings::Shadow::SHADOW_MAP) {
    DrawModelsWithProgramShadowMap(UseProgram(ProgramType::SHADOW_MAP));
  }

  for (RenderingModel& model : store_.models) {
    DrawModelWithProgramDefault(model, UseProgram(ProgramType::DEFAULT));
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

void RenderingEngineDefault::DrawModelWithProgramDefault(RenderingModel& model,
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
    UpdateUniform1iv(program, "pcf_kernel", settings_.pcf_kernel);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, store_.shadow_map.shadow_texture);

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, store_.g_buffer.g_position);

    UpdateUniform1fv(program, "enable_shadow_map", 1.0f);
  } else {
    UpdateUniform1fv(program, "enable_shadow_map", 0.0f);
  }

  if (settings_.ao == RenderingSettings::AO::SSAO) {
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, store_.ssao.color_buffer);

    int width, height;
    glfwGetFramebufferSize(window_, &width, &height);
    float screen_size[] = {(float)width, (float)height};
    UpdateUniform2fv(program, "screen_size", screen_size);

    UpdateUniform1fv(program, "enable_ssao", 1.0f);
  } else {
    UpdateUniform1fv(program, "enable_ssao", 0.0f);
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

  model.Draw();

  Logger::PrintProgramLog(program);
  glUseProgram(0);
}

void RenderingEngineDefault::DrawModelsWithProgramShadowMap(GLuint program) {
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

    model.Draw();
  }

  glDisable(GL_POLYGON_OFFSET_FILL);

  // 使用显示缓冲区, 重新开启绘制
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glDrawBuffer(GL_FRONT);

  Logger::PrintProgramLog(program);
  glUseProgram(0);
}

void RenderingEngineDefault::DrawModelsWithProgramGBuffer(GLuint program) {
  auto& g_buffer = store_.g_buffer;
  if (g_buffer.g_buffer == 0) {
    glGenFramebuffers(1, &g_buffer.g_buffer);
    glGenTextures(1, &g_buffer.g_position);
    glGenTextures(1, &g_buffer.g_normal);
    glGenTextures(1, &g_buffer.g_color);
    glGenTextures(1, &g_buffer.g_depth);
  }

  int width, height;
  glfwGetFramebufferSize(window_, &width, &height);

  { glBindFramebuffer(GL_FRAMEBUFFER, g_buffer.g_buffer); }

  {
    glBindTexture(GL_TEXTURE_2D, g_buffer.g_position);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA,
                 GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                           g_buffer.g_position, 0);
  }

  {
    glBindTexture(GL_TEXTURE_2D, g_buffer.g_normal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB,
                 GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D,
                           g_buffer.g_normal, 0);
  }

  {
    glBindTexture(GL_TEXTURE_2D, g_buffer.g_color);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_FLOAT,
                 NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D,
                           g_buffer.g_color, 0);
  }

  {
    glBindTexture(GL_TEXTURE_2D, g_buffer.g_depth);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height, 0,
                 GL_DEPTH_COMPONENT, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE,
                    GL_COMPARE_REF_TO_TEXTURE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, g_buffer.g_depth,
                         0);
  }

  GLuint attachments[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1,
                          GL_COLOR_ATTACHMENT2};
  glDrawBuffers(3, attachments);

  glEnable(GL_CULL_FACE);
  glClear(GL_DEPTH_BUFFER_BIT);

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);

  UpdateUniform1fv(program, "z_near", store_.z_near);
  UpdateUniform1fv(program, "z_far", store_.z_far);

  for (auto& model : store_.models) {
    glm::mat4 mv_matrix = store_.view_matrix * model.model_mat;
    glm::mat4 inv_tr_matrix = glm::transpose(glm::inverse(mv_matrix));
    UpdateUniformMat4fv(program, "mv_matrix", mv_matrix);
    UpdateUniformMat4fv(program, "proj_matrix", store_.perspective_matrix);
    UpdateUniformMat4fv(program, "norm_matrix", inv_tr_matrix);

    model.Draw();
  }

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glDrawBuffer(GL_FRONT);
}

void RenderingEngineDefault::GenerateSSAOSamplesAndNoise() {
  std::uniform_real_distribution<GLfloat> random_floats(0.0, 1.0);
  std::default_random_engine generator;
  std::vector<glm::vec3> ssao_kernel;

  auto lerp = [](GLfloat a, GLfloat b, GLfloat f) -> GLfloat {
    return a + f * (b - a);
  };

  for (GLuint i = 0; i < settings_.ssao_samples; i++) {
    std::uniform_real_distribution<GLfloat> random_floats2(0.00, 1);
    glm::vec3 sample(random_floats2(generator), random_floats2(generator),
                     random_floats2(generator));
    GLfloat scale = GLfloat(i + 1) / settings_.ssao_samples;
    scale = lerp(0.0f, 1.0f, scale * scale);
    sample *= scale;
    ssao_kernel.push_back(sample);
  }

  store_.ssao.samples = ssao_kernel;

  std::vector<glm::vec3> ssao_noise;
  for (GLuint i = 0; i < settings_.ssao_noise_samples; i++) {
    glm::vec3 noise(random_floats(generator) * 2.0 - 1.0,
                    random_floats(generator) * 2.0 - 1.0, 0.0f);
    ssao_noise.push_back(noise);
  }
}

void RenderingEngineDefault::DrawModelsWidthProgramSSAO(GLuint program) {
  if (!store_.ssao.initialized) {
    GenerateSSAOSamplesAndNoise();
    store_.ssao.initialized = true;
  }

  if (store_.ssao.fbo == 0) {
    glGenFramebuffers(1, &store_.ssao.fbo);
    glGenTextures(1, &store_.ssao.color_buffer);
    glGenTextures(1, &store_.ssao.debug_texture);
  }

  int width, height;
  glfwGetFramebufferSize(window_, &width, &height);

  float screen_size[] = {(float)width, (float)height};
  UpdateUniform2fv(program, "screen_size", screen_size);

  {
    glBindFramebuffer(GL_FRAMEBUFFER, store_.ssao.fbo);
    glBindTexture(GL_TEXTURE_2D, store_.ssao.color_buffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R16F, width, height, 0, GL_RED, GL_FLOAT,
                 NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                           store_.ssao.color_buffer, 0);
  }

  {
    glBindTexture(GL_TEXTURE_2D, store_.ssao.debug_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB,
                 GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D,
                           store_.ssao.debug_texture, 0);
  }

  glClear(GL_COLOR_BUFFER_BIT);
  glClear(GL_DEPTH_BUFFER_BIT);

  assert(settings_.need_gbuffer);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, store_.g_buffer.g_position);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, store_.g_buffer.g_normal);

  UpdateUniformArrayVec3fv(program, "samples", store_.ssao.samples);
  float noise_scale[] = {width / 4.0f, height / 4.0f};
  UpdateUniform2fv(program, "noise_scale", noise_scale);

  glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                       store_.g_buffer.g_depth, 0);

  GLuint buffers[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
  glDrawBuffers(2, buffers);

  glEnable(GL_CULL_FACE);

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);

  for (auto& model : store_.models) {
    glm::mat4 mv_matrix = store_.view_matrix * model.model_mat;
    glm::mat4 inv_tr_matrix = glm::transpose(glm::inverse(mv_matrix));
    UpdateUniformMat4fv(program, "mv_matrix", mv_matrix);
    UpdateUniformMat4fv(program, "proj_matrix", store_.perspective_matrix);
    UpdateUniformMat4fv(program, "norm_matrix", inv_tr_matrix);

    model.Draw();
  }

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glDrawBuffer(GL_FRONT);
  glUseProgram(0);
}

}  // namespace engine