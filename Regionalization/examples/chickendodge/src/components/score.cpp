#include "chickendodge/pch/precomp.h"

#include "chickendodge/components/score.h"

using json = nlohmann::json;

using namespace SimpleGE;

namespace ChickenDodge
{
  static void from_json(const json& j, ScoreComponent::Description& desc)
  {
    j.at("scoreSprite").get_to(desc.scoreSprite);
  }

  Component::SetupResult ScoreComponent::Setup(const nlohmann::json& descr) { return Setup(descr.get<Description>()); }

  Component::SetupResult ScoreComponent::Setup(const Description& descr)
  {
    scoreSprite = descr.scoreSprite;
    return {this, {{[this]() { return scoreSprite.Ready(); }, [this]() { SetValue(0); }}}};
  }

  void ScoreComponent::SetValue(int value)
  {
    this->value = value;
    scoreChangedEvent.Trigger(value);

    scoreSprite->SetText(fmt::format("{}", value));
  }
} // namespace ChickenDodge
