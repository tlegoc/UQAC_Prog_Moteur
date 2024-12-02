#include "chickendodge/pch/precomp.h"

#include "chickendodge/components/player.h"

#include "chickendodge/components/chicken.h"
#include "chickendodge/components/heart.h"
#include "chickendodge/components/life.h"
#include "chickendodge/components/rupee.h"
#include "chickendodge/components/score.h"

using json = nlohmann::json;

using namespace SimpleGE;

namespace ChickenDodge
{
  static std::vector<std::string> g_registeredNames;
  void PlayerComponent::SetNames(std::initializer_list<std::string> names)
  {
    std::copy(names.begin(), names.end(), std::back_inserter(g_registeredNames));
  }

  static void from_json(const json& j, PlayerComponent::Description& desc)
  {
    j.at("input").get_to(desc.input);
    j.at("prefix").get_to(desc.prefix);
    j.at("score").get_to(desc.score);
    j.at("life").get_to(desc.life);
    j.at("gameArea").get_to(desc.gameArea);
    j.at("hurtMotion").get_to(desc.hurtMotion);
    j.at("speed").get_to(desc.speed);
    j.at("onHurtEnable").get_to(desc.onHurtEnable);
    desc.invulnerableDuration = std::chrono::milliseconds{j.at("invulnerableDuration").get<int>()};
    desc.hurtDuration = std::chrono::milliseconds{j.at("hurtDuration").get<int>()};
  }

  static constexpr std::array<std::string_view, 4> FacingDir{"B", "F", "L", "R"};

  Component::SetupResult PlayerComponent::Setup(const nlohmann::json& descr) { return Setup(descr.get<Description>()); }

  Component::SetupResult PlayerComponent::Setup(const Description& descr)
  {
    static int playerIndex = 0;
    if (playerIndex < std::size(g_registeredNames))
    {
      SetName(gsl::at(g_registeredNames, playerIndex));
    }
    playerIndex++;

    prefix = descr.prefix;
    gameArea = descr.gameArea;
    invulnerableDuration = descr.invulnerableDuration;
    hurtDuration = descr.hurtDuration;
    hurtMotion = descr.hurtMotion;

    input = descr.input;
    score = descr.score;
    life = descr.life;
    speed = descr.speed;

    return {
        this,
        {{[this]() { return life.Ready(); }, [this, descr]() { SetupWithLife(descr); }},
         {[this]() { return Owner().GetComponent<SpriteComponent>() != nullptr; }, [this]() { SetupWithSprite(); }}}};
  }

  void PlayerComponent::SetupWithLife(const Description& descr)
  {
    life->RegisterDeadEvent([this]() { OnDead(); });
    life->RegisterHurtEvent([this]() { OnHurt(); });
    for (const auto& comp : descr.onHurtEnable)
    {
      life->RegisterHurtEvent(
          [this, comp]()
          {
            if (isLocal)
            {
              comp->Enable();
            }
          });
    }
  }

  void PlayerComponent::SetupWithSprite()
  {
    sprite = Owner().GetComponent<SpriteComponent>();
    Ensures(sprite != nullptr);
    sprite->RegisterAnimationEndedEvent(
        [this]()
        {
          isAttacking = false;
          sprite->SetFrameSkip(2);
          UpdateSprite();
          sprite->UpdateMesh();
        });

    UpdateSprite();
  }

  void PlayerComponent::UpdateLogic(const Timing& timing)
  {
    UpdatePendingActions(timing);

    Vector<3> delta;
    if (isDead)
    {
      delta = UpdateDead();
    }
    else if (isHurt)
    {
      delta = UpdateHurt();
    }
    else
    {
      delta = UpdateStandard();
    }

    bool visible = (!isInvulnerable) || (timing.frame % 2 == 0);
    sprite->SetEnabled(visible);

    gsl::not_null<PositionComponent*> position = Owner().GetComponent<PositionComponent>();

    delta *= std::chrono::duration_cast<std::chrono::duration<float>>(timing.delta).count();
    position->Translate(delta);
    position->Clamp(Point<2>({gameArea.xMin(), gameArea.yMin()}), Point<2>({gameArea.xMax(), gameArea.yMax()}));
  }

  void PlayerComponent::OnCollision(const SimpleGE::ColliderComponent& other)
  {
    auto& obj = other.Owner();
    RupeeComponent* rupee = obj.GetComponent<RupeeComponent>();
    HeartComponent* heart = obj.GetComponent<HeartComponent>();
    ChickenComponent* chicken = obj.GetComponent<ChickenComponent>();

    if (rupee != nullptr)
    {
      score->Increment(rupee->GetValue());
      obj.GetParent()->RemoveChild(obj);
    }
    if (heart != nullptr)
    {
      life->Increment(heart->GetHeal());
      obj.GetParent()->RemoveChild(obj);
    }
    if (chicken != nullptr)
    {
      if (isAttacking)
      {
        chicken->OnAttack();
      }
      else
      {
        life->Decrement(chicken->GetAttack());
      }
    }
  }

  void PlayerComponent::UpdatePendingActions(const Timing& timing)
  {
    using namespace std::chrono_literals;

    std::vector<int> elapsed;
    for (auto& pending : pendingActions)
    {
      auto actionId = pending.first;
      auto& remaining = pending.second.first;
      const auto& action = pending.second.second;

      remaining -= timing.delta;

      if (remaining <= 0ms)
      {
        elapsed.push_back(actionId);
        action();
      }
    }

    for (auto actionId : elapsed)
    {
      pendingActions.erase(actionId);
    }
  }

  void PlayerComponent::OnDead()
  {
    isDead = true;
    deadEvent.Trigger();
  }

  void PlayerComponent::OnHurt()
  {
    gsl::not_null<ColliderComponent*> collider = Owner().GetComponent<ColliderComponent>();

    isHurt = true;
    pendingActions[nextPendingAction++] = {hurtDuration, [this]() { isHurt = false; }};

    isInvulnerable = true;
    collider->Disable();
    pendingActions[nextPendingAction++] = {invulnerableDuration, [this, collider]()
                                           {
                                             isInvulnerable = false;
                                             collider->Enable();
                                           }};
  }

  void PlayerComponent::UpdateSprite()
  {
    using namespace std::literals::string_view_literals;

    bool isAnimated = isMoving || isAttacking;
    sprite->SetIsAnimated(isAnimated);
    std::string_view mod = isAttacking ? "A"sv : "M"sv;
    std::string_view frame = isAnimated ? ""sv : "1"sv;

    sprite->SetSpriteName(fmt::format("{}{}{}{}", prefix, mod, gsl::at(FacingDir, (int) facing), frame));
  }

  Vector<3> PlayerComponent::UpdateDead()
  {
    isMoving = false;
    isAttacking = false;
    sprite->SetIsAnimated(false);
    sprite->SetSpriteName(fmt::format("{}D", prefix));
    sprite->UpdateMesh();

    gsl::not_null<ColliderComponent*> collider = Owner().GetComponent<ColliderComponent>();
    collider->Disable();

    return {};
  }

  Vector<3> PlayerComponent::UpdateHurt()
  {
    isMoving = false;
    isAttacking = false;
    sprite->SetIsAnimated(false);
    sprite->SetSpriteName(fmt::format("{}H{}", prefix, gsl::at(FacingDir, (int) facing)));
    sprite->UpdateMesh();

    Vector<3> delta{};

    switch (facing)
    {
    case Facing::Back:
      delta[1] = hurtMotion;
      break;
    case Facing::Front:
      delta[1] = -hurtMotion;
      break;
    case Facing::Left:
      delta[0] = hurtMotion;
      break;
    case Facing::Right:
      delta[0] = -hurtMotion;
      break;
    }
    return delta;
  }

  Vector<3> PlayerComponent::UpdateStandard()
  {
    if (!isAttacking && input->IsActive(InputComponent::Action::Attack))
    {
      isAttacking = true;
      sprite->SetAnimationFrame(1);
      sprite->SetFrameSkip(1);
    }

    Vector<3> delta{};

    if (input->IsActive(InputComponent::Action::Up))
    {
      delta[1]--;
      facing = Facing::Back;
    }
    if (input->IsActive(InputComponent::Action::Down))
    {
      delta[1]++;
      facing = Facing::Front;
    }
    if (input->IsActive(InputComponent::Action::Left))
    {
      delta[0]--;
      facing = Facing::Left;
    }
    if (input->IsActive(InputComponent::Action::Right))
    {
      delta[0]++;
      facing = Facing::Right;
    }

    delta *= speed;

    isMoving = delta.MagnitudeSq() > 0;

    UpdateSprite();
    sprite->UpdateMesh();

    return delta;
  }
} // namespace ChickenDodge
