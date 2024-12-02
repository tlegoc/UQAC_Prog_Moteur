#include <simplege/pch/precomp.h>

#include <simplege/simplege.h>

using json = nlohmann::json;

namespace SimpleGE
{
  static void from_json(const json& j, ColliderComponent::Description& desc)
  {
    j.at("flag").get_to(desc.flag);
    j.at("mask").get_to(desc.mask);
    j.at("size").get_to(desc.size);
    if (j.contains("handler"))
    {
      j.at("handler").get_to(desc.handler);
    }
  }

  ColliderComponent::ColliderComponent(Entity& owner) : Component(owner) { PhysicSystem::Instance().Register(this); }

  ColliderComponent::~ColliderComponent() { PhysicSystem::Instance().Unregister(this); }

  Component::SetupResult ColliderComponent::Setup(const nlohmann::json& descr)
  {
    return Setup(descr.get<Description>());
  }

  Component::SetupResult ColliderComponent::Setup(const Description& descr)
  {
    flag = descr.flag;
    mask = descr.mask;
    size = descr.size;
    handler = descr.handler;

    return {this, {}};
  }

  [[nodiscard]] bool ColliderComponent::Collides(const ColliderComponent& other) const
  {
    return GetArea().Intersects(other.GetArea());
  }

  void ColliderComponent::OnCollision(const ColliderComponent& other) const
  {
    auto* collHandler = handler.Resolve();
    if (collHandler != nullptr)
    {
      collHandler->OnCollision(other);
    }
  }

  [[nodiscard]] Area ColliderComponent::GetArea() const
  {
    gsl::not_null<PositionComponent*> posComp = Owner().GetComponent<PositionComponent>();
    const auto position = posComp->WorldPosition();
    return Area{position[0], position[1], float(size.width), float(size.height)};
  }

} // namespace SimpleGE
