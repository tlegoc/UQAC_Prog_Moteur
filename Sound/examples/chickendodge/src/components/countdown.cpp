#include "chickendodge/pch/precomp.h"

#include "chickendodge/components/countdown.h"

#include "chickendodge/components/networkplayermanager.h"

using json = nlohmann::json;

using namespace SimpleGE;

namespace ChickenDodge
{
  static void from_json(const json& j, CountdownComponent::Description& desc)
  {
    j.at("sprites").get_to(desc.sprites);
    desc.spriteTemplate = j.at("spriteTemplate");
    desc.delay = std::chrono::milliseconds{j.at("delay").get<int>()};

    if (j.contains("waitSprite"))
    {
      j.at("waitSprite").get_to(desc.waitSprite);
    }
    if (j.contains("playerSpritePrefix"))
    {
      j.at("playerSpritePrefix").get_to(desc.playerSpritePrefix);
    }
    if (j.contains("playerWait"))
    {
      j.at("playerWait").get_to(desc.playerWait);
    }
  }

  Component::SetupResult CountdownComponent::Setup(const nlohmann::json& descr)
  {
    return Setup(descr.get<Description>());
  }

  Component::SetupResult CountdownComponent::Setup(const Description& descr)
  {
    std::transform(descr.sprites.begin(), descr.sprites.end(), std::back_inserter(sprites),
                   [](const auto& s) { return Localisation::Get(s); });
    waitSprite = Localisation::Get(descr.waitSprite);
    playerSpritePrefix = descr.playerSpritePrefix;
    delay = descr.delay;
    spriteTemplate = descr.spriteTemplate;

    if (descr.playerWait.IsEmpty())
    {
      return {this, {}};
    }

    return {this, {{[descr]() { return descr.playerWait.Ready(); }, [this, descr]() { SetupImpl(descr); }}}};
  }

  void CountdownComponent::SetupImpl(const Description& descr)
  {
    ShowNamedImage(waitSprite);
    Disable();
    descr.playerWait->RegisterPlayerReadyEvent([this](int localIndex) { OnPlayerReady(localIndex); });
  }

  void CountdownComponent::UpdateLogic(const Timing& timing)
  {
    shownTime += timing.delta;
    if (shownTime < delay)
    {
      return;
    }

    index++;
    if (current != nullptr)
    {
      Owner().RemoveChild(current);
      current.reset();
    }

    if (index >= std::size(sprites))
    {
      auto* enabler = Owner().GetComponent<EnablerComponent>();
      if (enabler != nullptr)
      {
        enabler->OnEvent();
      }
      Disable();
    }
    else
    {
      ShowImage();
    }
  }

  void CountdownComponent::ShowImage()
  {
    shownTime = std::chrono::milliseconds::zero();
    ShowNamedImage(gsl::at(sprites, index));

    AudioSystem::SendEvent("countdown");
  }

  void CountdownComponent::ShowNamedImage(const std::string& textureName)
  {
    spriteTemplate.at("components").at("RawSprite")["texture"] = textureName;
    current = Scene::CreateChild(spriteTemplate, "sprite", Owner().shared_from_this());
  }

  void CountdownComponent::OnPlayerReady(int localIndex)
  {
    auto sprite = Localisation::Get(fmt::format("{}{}", playerSpritePrefix, localIndex));
    sprites.insert(sprites.begin(), sprite);
    Enable();
  }
} // namespace ChickenDodge
