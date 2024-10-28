#pragma once

#include "chickendodge/pch/precomp.h"

namespace ChickenDodge
{
  class LifeComponent : public SimpleGE::Component
  {
  public:
    static constexpr auto Type = "Life";

    using DeadEventType = std::function<void()>;
    using HurtEventType = std::function<void()>;

    enum class HeartSprites
    {
      Empty = 0,
      Half = 1,
      Full = 2,

      Count = Full + 1
    };

    struct Description
    {
      int max{};
      float count{};
      SimpleGE::ComponentReference<SimpleGE::TextSpriteComponent> lifeSprite;
      std::array<std::string, (int) HeartSprites::Count> sprites;
    };

    LifeComponent(SimpleGE::Entity& owner) : Component(owner) {}

    SetupResult Setup(const nlohmann::json& descr) override;
    SetupResult Setup(const Description& descr);

    float GetValue() const { return value; }
    void SetValue(float value);

    void Increment(float value) { SetValue(GetValue() + value); }
    void Decrement(float value) { SetValue(GetValue() - value); }

    void RegisterDeadEvent(const DeadEventType& onDead) { deadEvent.Register(onDead); }
    void RegisterHurtEvent(const HurtEventType& onHurt) { hurtEvent.Register(onHurt); }

  private:
    SimpleGE::EventTrigger<LifeComponent::DeadEventType> deadEvent;
    SimpleGE::EventTrigger<LifeComponent::HurtEventType> hurtEvent;
    int max{};
    float value{};
    decltype(LifeComponent::Description::sprites) sprites;
    SimpleGE::ComponentReference<SimpleGE::TextSpriteComponent> lifeSprite;
  };
} // namespace ChickenDodge