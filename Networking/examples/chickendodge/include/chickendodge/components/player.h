#pragma once

#include "chickendodge/pch/precomp.h"

namespace ChickenDodge
{
  class LifeComponent;
  class ScoreComponent;

  class PlayerComponent : public SimpleGE::LogicComponent, public SimpleGE::CollisionComponent
  {
  public:
    static constexpr auto Type = "Player";

    using DeadEventType = std::function<void()>;

    struct Description
    {
      SimpleGE::ComponentReference<SimpleGE::InputComponent> input;
      std::string prefix;
      SimpleGE::ComponentReference<ScoreComponent> score;
      SimpleGE::ComponentReference<LifeComponent> life;
      SimpleGE::Area gameArea;
      std::chrono::milliseconds invulnerableDuration;
      std::chrono::milliseconds hurtDuration;
      float hurtMotion;
      float speed;
      std::vector<SimpleGE::ComponentReference<SimpleGE::Component>> onHurtEnable;
    };

    static void SetNames(std::initializer_list<std::string> names);

    PlayerComponent(SimpleGE::Entity& owner) : Component(owner) {}

    SetupResult Setup(const nlohmann::json& descr) override;
    SetupResult Setup(const Description& descr);

    void UpdateLogic(const SimpleGE::Timing& timing) override;
    void OnCollision(const SimpleGE::ColliderComponent& other) override;

    void SetLocal(bool isLocal) { this->isLocal = isLocal; }
    void SetName(std::string_view name) { this->name = name; }
    [[nodiscard]] std::string_view GetName() const { return name; }
    [[nodiscard]] bool IsDead() const { return isDead; }
    [[nodiscard]] const SimpleGE::ComponentReference<ScoreComponent>& GetScore() const { return score; }

    void RegisterDeadEvent(const DeadEventType& onDead) { deadEvent.Register(onDead); }

  private:
    void SetupWithLife(const Description& descr);
    void SetupWithSprite();

    void UpdatePendingActions(const SimpleGE::Timing& timing);
    void OnDead();
    void OnHurt();
    void UpdateSprite();
    SimpleGE::Vector<3> UpdateDead();
    SimpleGE::Vector<3> UpdateHurt();
    SimpleGE::Vector<3> UpdateStandard();

    enum class Facing
    {
      Back,
      Front,
      Left,
      Right
    };

    SimpleGE::EventTrigger<PlayerComponent::DeadEventType> deadEvent;
    bool isDead{};
    SimpleGE::ComponentReference<ScoreComponent> score;
    std::string name;
    std::string prefix;
    bool isLocal{true};

    SimpleGE::Area gameArea;
    Facing facing{Facing::Front};
    bool isAttacking{};
    bool isMoving{};
    bool isHurt{};
    bool isInvulnerable{};
    std::chrono::milliseconds invulnerableDuration{};
    std::chrono::milliseconds hurtDuration{};
    float hurtMotion{};
    float speed{};
    SimpleGE::ComponentReference<SimpleGE::InputComponent> input;
    SimpleGE::ComponentReference<LifeComponent> life;
    SimpleGE::SpriteComponent* sprite{};

    std::unordered_map<int, std::pair<std::chrono::milliseconds, std::function<void()>>> pendingActions;
    int nextPendingAction{};
  };
} // namespace ChickenDodge