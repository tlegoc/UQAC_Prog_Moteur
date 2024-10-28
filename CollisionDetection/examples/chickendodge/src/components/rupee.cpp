#include "chickendodge/pch/precomp.h"

#include "chickendodge/components/rupee.h"

using json = nlohmann::json;

using namespace SimpleGE;

namespace ChickenDodge
{
  static void from_json(const json& j, RupeeComponent::Description& desc)
  {
    j.at("values").get_to(desc.values);
    desc.lifetime = std::chrono::milliseconds{j.at("lifetime").get<int>()};
  }

  Component::SetupResult RupeeComponent::Setup(const nlohmann::json& descr) { return Setup(descr.get<Description>()); }

  Component::SetupResult RupeeComponent::Setup(const Description& descr)
  {
    int randVal = std::rand() % std::size(descr.values);
    const auto randomType = std::next(descr.values.begin(), randVal);
    type = randomType->first;
    value = randomType->second;
    remaining = descr.lifetime;

    return {this, {{[this]() { return Owner().GetComponent<SpriteComponent>()->Ready(); }, [this]() { SetupImpl(); }}}};
  }

  void RupeeComponent::SetupImpl()
  {
    gsl::not_null<SpriteComponent*> sprite = Owner().GetComponent<SpriteComponent>();
    sprite->SetSpriteName(type);
    sprite->SetAnimationFrame(1);
    sprite->UpdateMesh();
  }

  void RupeeComponent::UpdateLogic(const Timing& timing)
  {
    using namespace std::chrono_literals;

    remaining -= timing.delta;
    if (remaining <= 0ms)
    {
      auto& owner = Owner();
      owner.GetParent()->RemoveChild(owner);
    }
  }
} // namespace ChickenDodge
