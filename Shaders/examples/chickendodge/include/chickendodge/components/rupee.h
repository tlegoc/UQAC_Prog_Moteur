#pragma once

#include "chickendodge/pch/precomp.h"

namespace ChickenDodge
{
  class RupeeComponent : public SimpleGE::LogicComponent
  {
  public:
    static constexpr auto Type = "Rupee";

    struct Description
    {
      std::unordered_map<std::string, int> values;
      std::chrono::milliseconds lifetime;
    };

    RupeeComponent(SimpleGE::Entity& owner) : Component(owner) {}

    SetupResult Setup(const nlohmann::json& descr) override;
    SetupResult Setup(const Description& descr);

    void UpdateLogic(const SimpleGE::Timing& timing) override;

    [[nodiscard]] int GetValue() const { return value; }

  private:
    void SetupImpl();

    std::string type{};
    int value{};
    std::chrono::milliseconds remaining{};
  };
} // namespace ChickenDodge