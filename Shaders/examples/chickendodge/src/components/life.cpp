#include "chickendodge/pch/precomp.h"

#include "chickendodge/components/life.h"

using json = nlohmann::json;

using namespace SimpleGE;

namespace ChickenDodge
{
  static void from_json(const json& j, LifeComponent::Description& desc)
  {
    j.at("max").get_to(desc.max);
    desc.count = float(desc.max);
    if (j.contains("default"))
    {
      j.at("default").get_to(desc.count);
    }
    j.at("lifeSprite").get_to(desc.lifeSprite);
    j.at("sprites").get_to(desc.sprites);
  }

  Component::SetupResult LifeComponent::Setup(const nlohmann::json& descr) { return Setup(descr.get<Description>()); }

  Component::SetupResult LifeComponent::Setup(const Description& descr)
  {
    max = descr.max;
    sprites = descr.sprites;
    lifeSprite = descr.lifeSprite;

    return {this, {{[this]() { return lifeSprite.Ready(); }, [this, descr]() { SetValue(descr.count); }}}};
  }

  void LifeComponent::SetValue(float value)
  {
    if (value < 0.F)
    {
      value = 0.F;
    }
    if (value > float(max))
    {
      value = float(max);
    }

    if (value == 0.F)
    {
      deadEvent.Trigger();
    }
    else if (value < GetValue())
    {
      hurtEvent.Trigger();
    }

    this->value = value;
    std::vector<std::string_view> hearts;
    hearts.reserve(max);
    for (int i = 0; i < max; i++)
    {
      HeartSprites sIndex = HeartSprites::Empty;
      if (float(i) < value)
      {
        sIndex = HeartSprites::Half;
      }
      if (float(i + 1) <= value)
      {
        sIndex = HeartSprites::Full;
      }
      hearts.push_back(sprites[(int) sIndex]);
    }

    lifeSprite->SetCharacters(hearts);
  }
} // namespace ChickenDodge
