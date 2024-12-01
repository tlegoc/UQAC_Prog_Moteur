#pragma once

#include "chickendodge/pch/precomp.h"

namespace ChickenDodge
{
  class TimerComponent : public SimpleGE::LogicComponent
  {
  public:
    static constexpr auto Type = "Timer";

    struct Description
    {
    };

    TimerComponent(SimpleGE::Entity& owner) : Component(owner) {}

    SetupResult Setup(const nlohmann::json& descr) override;
    SetupResult Setup(const Description& descr);

    void UpdateLogic(const SimpleGE::Timing& timing) override;

  protected:
    void OnEnabled() override { paused = false; }
    void OnDisabled() override { paused = true; }

  private:
    SimpleGE::TextSpriteComponent* textSprite{};
    std::chrono::milliseconds elapsed{};
    bool paused{};
  };
} // namespace ChickenDodge