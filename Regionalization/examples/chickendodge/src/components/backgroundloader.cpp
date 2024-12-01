#include "chickendodge/pch/precomp.h"

#include "chickendodge/components/backgroundloader.h"

#include <sstream>

using json = nlohmann::json;

using namespace SimpleGE;

namespace ChickenDodge
{
  static void from_json(const json& j, BackgroundLoaderComponent::Entry& entry)
  {
    j.at("spriteName").get_to(entry.spriteName);
    j.at("isAnimated").get_to(entry.isAnimated);
    j.at("frameSkip").get_to(entry.frameSkip);
  }

  static void from_json(const json& j, BackgroundLoaderComponent::Description& desc)
  {
    j.at("description").get_to(desc.description);
    j.at("spriteSheet").get_to(desc.spriteSheet);
    j.at("scale").get_to(desc.scale);

    std::unordered_map<std::string, BackgroundLoaderComponent::Entry> entryMapStr;
    j.at("entryMap").get_to(entryMapStr);

    std::transform(entryMapStr.begin(), entryMapStr.end(), std::inserter(desc.entryMap, desc.entryMap.end()),
                   [](auto& pair) {
                     return BackgroundLoaderComponent::EntryMap::value_type{pair.first[0], std::move(pair.second)};
                   });
  }

  // ## Méthode *setup*
  // Cette méthode est responsable d'instancier les différents
  // objets contenant des sprites.
  Component::SetupResult BackgroundLoaderComponent::Setup(const nlohmann::json& descr)
  {
    return Setup(descr.get<Description>());
  }

  Component::SetupResult BackgroundLoaderComponent::Setup(const Description& descr)
  {
    return {this, {{[descr]() { return descr.spriteSheet.Ready(); }, [this, descr]() { SetupImpl(descr); }}}};
  }

  void BackgroundLoaderComponent::SetupImpl(const Description& descr)
  {
    auto content = Resources::Get<TextAsset>(descr.description);
    Ensures(content != nullptr);

    std::istringstream stream((std::string(*content)));
    std::string line;
    for (int row = 0; std::getline(stream, line); row++)
    {
      for (int col = 0; col < line.length(); col++)
      {
        auto entryIt = descr.entryMap.find(line[col]);
        if (entryIt == descr.entryMap.end())
        {
          continue;
        }

        const auto& entry = entryIt->second;

        constexpr auto DepthSkew = 0.01F;

        Scene::CreateChild<PositionComponent, SpriteComponent>(
            {float(col) * descr.scale, float(row) * descr.scale, float(row) * DepthSkew},
            {entry.frameSkip, entry.isAnimated, 0, entry.spriteName, descr.spriteSheet}, fmt::format("{}-{}", col, row),
            Owner().shared_from_this());
      }
    }
  }
} // namespace ChickenDodge
