#include <simplege/pch/precomp.h>

#include <simplege/simplege.h>

#include <thread>

namespace SimpleGE
{
  using namespace std::chrono_literals;
  static constexpr auto FrameLimiter = 25ms;

  Game::~Game() { Scene::Clear(); }

  Game::RunResult Game::Run()
  {
    RegisterGenericComponents();
    RegisterComponents();
    RegisterGenericMessages();
    RegisterMessages();
    SetupSystem();

    for (const auto& system : systems)
    {
      if (dynamic_cast<const DisplaySystem*>(system.get()) != nullptr)
      {
        RegisterGenericShaderPrograms();
        RegisterShaderPrograms();
      }
    }

    AddSystem<EntitySystem>();

    if (!LoadScene(GetLaunchScene()))
    {
      return RunResult::Failure;
    }

    auto lastTime = std::chrono::high_resolution_clock::now();
    for (unsigned int frame = 0; !CloseRequested(); frame++)
    {
      auto now = std::chrono::high_resolution_clock::now();
      auto delta = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastTime);
      if (delta < FrameLimiter)
      {
        std::this_thread::sleep_for(FrameLimiter - delta);
        now = std::chrono::high_resolution_clock::now();
        delta = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastTime);
      }
      lastTime = now;

      frameBeginEvent.Trigger(frame);
      for (auto& system : systems)
      {
        system->Iterate({delta, frame});
      }
      frameEndEvent.Trigger(frame);
    }

    return RunResult::Success;
  }

  bool Game::LoadScene(std::string_view file)
  {
    auto content = Resources::Get<TextAsset>(file);
    if (content == nullptr)
    {
      return false;
    }
    Scene::Load(*content);
    return true;
  }
} // namespace SimpleGE