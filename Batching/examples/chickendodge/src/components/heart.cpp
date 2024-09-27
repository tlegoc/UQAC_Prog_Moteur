#include "chickendodge/pch/precomp.h"

#include "chickendodge/components/heart.h"

using json = nlohmann::json;

using namespace SimpleGE;

namespace ChickenDodge
{
  static void from_json(const json& j, HeartComponent::Description& desc)
  {
    j.at("heal").get_to(desc.heal);
    desc.lifetime = std::chrono::milliseconds{j.at("lifetime").get<int>()};
  }

  Component::SetupResult HeartComponent::Setup(const nlohmann::json& descr) { return Setup(descr.get<Description>()); }

  Component::SetupResult HeartComponent::Setup(const Description& descr)
  {
    heal = descr.heal;
    remaining = descr.lifetime;

    return {this, {}};
  }

  void HeartComponent::UpdateLogic(const Timing& timing)
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
