#include "chickendodge/pch/precomp.h"

#include "chickendodge/components/timer.h"

#include <fmt/chrono.h>

using json = nlohmann::json;

using namespace SimpleGE;

namespace ChickenDodge
{
  static void from_json(const json& j, TimerComponent::Description& desc) {}

  Component::SetupResult TimerComponent::Setup(const nlohmann::json& descr) { return Setup(descr.get<Description>()); }

  Component::SetupResult TimerComponent::Setup(const Description& descr)
  {
    textSprite = Owner().GetComponent<TextSpriteComponent>();
    Ensures(textSprite != nullptr);

    return {this, {}};
  }

  void TimerComponent::UpdateLogic(const Timing& timing)
  {
    using namespace std::chrono_literals;

    elapsed += timing.delta;

    auto elapsedSeconds = std::chrono::duration_cast<std::chrono::seconds>(elapsed);
    std::string elapsedStr;
    if (elapsed < 1h)
    {
      elapsedStr = fmt::format("{:%M:%S}", elapsedSeconds);
    }
    else
    {
      elapsedStr = fmt::format("{:%T}", elapsedSeconds);
    }

    std::vector<std::string_view> characters;
    characters.reserve(std::size(elapsedStr));
    for (const auto& c : elapsedStr)
    {
      if (c == ':')
      {
        characters.emplace_back("colon");
      }
      else
      {
        characters.emplace_back(&c, 1);
      }
    }

    textSprite->SetCharacters(characters);
  }
} // namespace ChickenDodge
