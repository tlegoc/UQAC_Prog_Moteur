#pragma once

#include "chickendodge/pch/precomp.h"

namespace ChickenDodge
{
  class HeartComponent : public SimpleGE::LogicComponent
  {
  public:
    static constexpr auto Type = "Heart";

    struct Description
    {
      float heal;
      std::chrono::milliseconds lifetime;
    };

    HeartComponent(SimpleGE::Entity& owner) : Component(owner) {}

    SetupResult Setup(const nlohmann::json& descr) override;
    SetupResult Setup(const Description& descr);

    void UpdateLogic(const SimpleGE::Timing& timing) override;

    [[nodiscard]] float GetHeal() const { return heal; }

  private:
    float heal{};
    std::chrono::milliseconds remaining{};
  };
} // namespace ChickenDodge