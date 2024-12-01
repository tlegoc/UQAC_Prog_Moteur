#include "chickendodge/pch/precomp.h"

#include "chickendodge/components/chicken.h"

using json = nlohmann::json;

using namespace SimpleGE;

namespace ChickenDodge
{
  static void from_json(const json& j, ChickenComponent::Description& desc)
  {
    j.at("attack").get_to(desc.attack);
    j.at("heartAttackChance").get_to(desc.heartAttackChance);
    desc.heartTemplate = j.at("heartTemplate");
    desc.rupeeTemplate = j.at("rupeeTemplate");
    j.at("target").get_to(desc.target);
  }

  Component::SetupResult ChickenComponent::Setup(const nlohmann::json& descr)
  {
    return Setup(descr.get<Description>());
  }

  Component::SetupResult ChickenComponent::Setup(const Description& descr)
  {
    target = descr.target;
    rupeeTemplate = descr.rupeeTemplate;
    heartAttackChance = descr.heartAttackChance;
    heartTemplate = descr.heartTemplate;
    attack = descr.attack;

    return {
        this,
        {{[this]() { return Owner().GetComponent<PositionComponent>()->Ready(); }, [this]() { SetupWithPosition(); }},
         {[this]() { return Owner().GetComponent<SpriteComponent>()->Ready(); }, [this]() { SetupWithSprite(); }}}};
  }

  void ChickenComponent::SetupWithPosition()
  {
    gsl::not_null<PositionComponent*> position = Owner().GetComponent<PositionComponent>();
    velocity = target - Point<2>(position->LocalPosition());
    velocity.Normalize();
    velocity *= float(std::rand()) / float(RAND_MAX) * 45.F + 30.F;
  }

  void ChickenComponent::SetupWithSprite()
  {
    using namespace std::literals::string_view_literals;
    gsl::not_null<SpriteComponent*> sprite = Owner().GetComponent<SpriteComponent>();
    std::string_view dir = (velocity[0] > 0.F) ? "R"sv : "L"sv;
    sprite->SetSpriteName(fmt::format("C{}", dir));
  }

  void ChickenComponent::UpdateLogic(const Timing& timing)
  {
    gsl::not_null<PositionComponent*> position = Owner().GetComponent<PositionComponent>();
    auto targetDistanceSq = (target - Point<2>(position->LocalPosition())).MagnitudeSq();

    auto delta = velocity * std::chrono::duration_cast<std::chrono::duration<float>>(timing.delta).count();
    position->Translate(Vector<3>(delta));

    auto newTargetDistanceSq = (target - Point<2>(position->LocalPosition())).MagnitudeSq();
    if ((!dropped) && (newTargetDistanceSq > targetDistanceSq))
    {
      Drop(rupeeTemplate);
    }

    distance += delta.Magnitude();
    if (distance > 500.F)
    {
      Owner().GetParent()->RemoveChild(Owner());
    }
  }

  void ChickenComponent::OnAttack()
  {
    auto& toDrop = (float(std::rand()) / float(RAND_MAX) < heartAttackChance) ? heartTemplate : rupeeTemplate;
    Drop(toDrop);

    gsl::not_null<ColliderComponent*> collider = Owner().GetComponent<ColliderComponent>();
    collider->Disable();
    velocity[0] *= -1.F;

    using namespace std::literals::string_view_literals;
    gsl::not_null<SpriteComponent*> sprite = Owner().GetComponent<SpriteComponent>();
    std::string_view dir = (velocity[0] > 0.F) ? "R"sv : "L"sv;
    sprite->SetSpriteName(fmt::format("C{}", dir));
  }

  void ChickenComponent::Drop(nlohmann::json dropTemplate)
  {
    static int dropId{};
    dropId++;

    gsl::not_null<PositionComponent*> position = Owner().GetComponent<PositionComponent>();
    auto localPos = position->LocalPosition();

    auto& posTpl = dropTemplate.at("components").at("Position");
    posTpl["x"] = localPos[0];
    posTpl["y"] = localPos[1];
    posTpl["z"] = localPos[2];

    Scene::CreateChild(dropTemplate, fmt::format("{}", dropId), Owner().GetParent());
    dropped = true;
  }
} // namespace ChickenDodge
