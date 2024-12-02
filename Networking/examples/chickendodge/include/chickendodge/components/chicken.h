#pragma once

#include "chickendodge/pch/precomp.h"

namespace ChickenDodge
{
  class ChickenComponent : public SimpleGE::LogicComponent
  {
  public:
    static constexpr auto Type = "Chicken";

    struct Description
    {
      float attack;
      float heartAttackChance;
      nlohmann::json heartTemplate;
      nlohmann::json rupeeTemplate;
      SimpleGE::Point<2> target;
    };

    ChickenComponent(SimpleGE::Entity& owner) : Component(owner) {}

    SetupResult Setup(const nlohmann::json& descr) override;
    SetupResult Setup(const Description& descr);

    void UpdateLogic(const SimpleGE::Timing& timing) override;

    void OnAttack();
    [[nodiscard]] float GetAttack() const { return attack; }

  private:
    void SetupWithPosition();
    void SetupWithSprite();

    void Drop(nlohmann::json dropTemplate);

    float attack{};
    bool dropped{};
    float distance{};
    SimpleGE::Point<2> target;
    float heartAttackChance{};
    nlohmann::json heartTemplate;
    nlohmann::json rupeeTemplate;
    SimpleGE::Vector<2> velocity;
  };
} // namespace ChickenDodge