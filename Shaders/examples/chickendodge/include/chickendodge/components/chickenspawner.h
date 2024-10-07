#pragma once

#include "chickendodge/pch/precomp.h"

namespace ChickenDodge
{
  class ChickenSpawnerComponent : public SimpleGE::LogicComponent
  {
  public:
    static constexpr auto Type = "ChickenSpawner";

    struct Description
    {
      SimpleGE::Area sourceArea;
      SimpleGE::Area targetArea;
      std::chrono::milliseconds spawnDelay;
      float spawnWaitFactor;
      nlohmann::json chickenTemplate;
    };

    ChickenSpawnerComponent(SimpleGE::Entity& owner);

    SetupResult Setup(const nlohmann::json& descr) override;
    SetupResult Setup(const Description& descr);

    void UpdateLogic(const SimpleGE::Timing& timing) override;

  private:
    void Spawn(const SimpleGE::Timing& timing);

    SimpleGE::Area sourceArea;
    SimpleGE::Area targetArea;
    std::chrono::milliseconds spawnDelay{};
    float spawnWaitFactor{};
    nlohmann::json chickenTemplate;
    std::chrono::milliseconds elapsed{};
  };
} // namespace ChickenDodge