#pragma once

#include <simplege/pch/precomp.h>

#include <simplege/components/component.h>

namespace SimpleGE
{
  class SpriteSheetComponent;

  class TextSpriteComponent : public Component
  {
  public:
    static constexpr auto Type = "TextSprite";

    enum class Align
    {
      Left,
      Right,
    };

    struct Description
    {
      ComponentReference<SpriteSheetComponent> spriteSheet;
      Align align{};
    };

    TextSpriteComponent(Entity& owner) : Component(owner) {}

    SetupResult Setup(const nlohmann::json& descr) override;
    SetupResult Setup(const Description& descr);

    void SetText(std::string_view text);
    void SetCharacters(std::span<std::string_view> array);

  private:
    void UpdateTextSprites();

    Align align{};
    ComponentReference<SpriteSheetComponent> spriteSheet;

    std::vector<EntityRef> sprites;
    std::vector<std::string> text;
  };
} // namespace SimpleGE
