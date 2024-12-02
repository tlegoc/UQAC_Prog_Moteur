#include <simplege/pch/precomp.h>

#include <simplege/simplege.h>

#include <fmt/ranges.h>

using json = nlohmann::json;

namespace SimpleGE
{
  static void from_json(const json& j, TextSpriteComponent::Description& desc)
  {
    desc.align = (j.at("align").get<std::string_view>() == "left") ? TextSpriteComponent::Align::Left
                                                                   : TextSpriteComponent::Align::Right;
    j.at("spriteSheet").get_to(desc.spriteSheet);
  }

  Component::SetupResult TextSpriteComponent::Setup(const nlohmann::json& descr)
  {
    return Setup(descr.get<Description>());
  }

  Component::SetupResult TextSpriteComponent::Setup(const Description& descr)
  {
    align = descr.align;
    spriteSheet = descr.spriteSheet;
    return {this, {{[this]() { return spriteSheet.Ready(); }, [this]() { UpdateTextSprites(); }}}};
  }

  void TextSpriteComponent::SetText(std::string_view text)
  {
    std::vector<std::string_view> characters;
    characters.reserve(std::size(text));
    for (const auto& c : text)
    {
      characters.emplace_back(&c, 1);
    }

    SetCharacters(characters);
  }

  void TextSpriteComponent::SetCharacters(std::span<std::string_view> array)
  {
    if (std::equal(text.begin(), text.end(), array.begin(), array.end(),
                   [](const auto& a, const auto& b) { return a == b; }))
    {
      return;
    }

    text.clear();
    std::transform(array.begin(), array.end(), std::back_inserter(text),
                   [](const auto& c) { return std::string(c.data(), std::size(c)); });

    UpdateTextSprites();
  }

  // On crée de nouvelles sprites pour chaque caractère de la
  // chaîne, on les positionne correctement, et on détruit les
  // anciens sprites.
  void TextSpriteComponent::UpdateTextSprites()
  {
    for (auto& s : sprites)
    {
      s->GetParent()->RemoveChild(s);
    }
    sprites.clear();

    std::vector<std::string> characters = text;
    if (align == TextSpriteComponent::Align::Right)
    {
      std::reverse(characters.begin(), characters.end());
    }
    float dir = (align == TextSpriteComponent::Align::Left) ? 1.F : -1.F;

    float offset{};
    int index = 0;
    for (const auto& c : characters)
    {
      const auto* glyph = spriteSheet->GetSprite(c);
      if (glyph == nullptr)
      {
        continue;
      }

      float x = offset;
      offset += float(glyph->sourceSize.width) * dir;

      auto newSpriteObj = Scene::CreateChild<PositionComponent, SpriteComponent>(
          {x, 0.F, 0.F}, {0, false, 0, c, spriteSheet}, fmt::format("{}_{}", fmt::join(text, ""), index),
          Owner().shared_from_this());

      sprites.emplace_back(newSpriteObj);

      index++;
    }
  }
} // namespace SimpleGE
