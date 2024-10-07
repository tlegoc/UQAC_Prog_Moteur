#pragma once

#include "chickendodge/pch/precomp.h"

namespace ChickenDodge
{
  class ScoreComponent : public SimpleGE::Component
  {
  public:
    static constexpr auto Type = "Score";

    using ScoreChangedEventType = std::function<void(int)>;

    struct Description
    {
      SimpleGE::ComponentReference<SimpleGE::TextSpriteComponent> scoreSprite;
    };

    ScoreComponent(SimpleGE::Entity& owner) : Component(owner) {}

    SetupResult Setup(const nlohmann::json& descr) override;
    SetupResult Setup(const Description& descr);

    [[nodiscard]] int GetValue() const { return value; }
    void SetValue(int value);

    void Increment(int value) { SetValue(GetValue() + value); }

    void Decrement(int value) { SetValue(GetValue() - value); }

    void RegisterScoreChangedEvent(const ScoreChangedEventType& onScoreChanged)
    {
      scoreChangedEvent.Register(onScoreChanged);
    }

  private:
    int value{};
    SimpleGE::EventTrigger<ScoreComponent::ScoreChangedEventType> scoreChangedEvent;
    SimpleGE::ComponentReference<SimpleGE::TextSpriteComponent> scoreSprite;
  };
} // namespace ChickenDodge