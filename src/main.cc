#include "RenderingEngine/RenderingEngineDefault.h"

int main() {
  auto& e =
      engine::RenderingEngine::Singleton<engine::RenderingEngineDefault>();
  e.Start();
}
