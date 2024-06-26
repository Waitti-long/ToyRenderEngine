#pragma once

namespace engine {

class RenderingSettings {
 public:
  enum class Shadow { NONE, SHADOW_MAP, VSM };

  enum class GI { NONE, RSM, LPV, VGI, SDFDDGI, SSR };

  enum class AO { NONE, SSAO, SSDO };

  Shadow shadow = Shadow::NONE;
  GI gi = GI::NONE;
  AO ao = AO::NONE;

  bool need_gbuffer = false;

  float ssao_samples = 64;
  float ssao_noise_samples = 16;

  int pcf_kernel = 0;

  bool test_compute_shader = false;
};

}  // namespace engine
