#include <simplege/pch/precomp.h>

#include <simplege/simplege.h>

using json = nlohmann::json;

namespace SimpleGE
{
  static void from_json(const json& j, PositionComponent::Description& desc)
  {
    j.at("x").get_to(desc.x);
    j.at("y").get_to(desc.y);
    j.at("z").get_to(desc.z);
  }

  Component::SetupResult PositionComponent::Setup(const nlohmann::json& descr)
  {
    return Setup(descr.get<Description>());
  }

  Component::SetupResult PositionComponent::Setup(const Description& descr)
  {
    localPosition[0] = descr.x;
    localPosition[1] = descr.y;
    localPosition[2] = descr.z;

    return {this, {}};
  }

  // Cette propriété combine les transformations des parents afin
  // de trouver la position absolue de l'objet dans le monde.
  [[nodiscard]] Point<3> PositionComponent::WorldPosition() const
  {
    auto pos = LocalPosition();
    auto parent = Owner().GetParent();
    if (parent == nullptr)
    {
      return pos;
    }

    const auto* parentPos = parent->GetComponent<PositionComponent>();
    if (parentPos == nullptr)
    {
      return pos;
    }

    return (pos - Point<3>::Origin()) + parentPos->WorldPosition();
  }
} // namespace SimpleGE