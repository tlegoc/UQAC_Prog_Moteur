#pragma once

#include <simplege/pch/precomp.h>

#include <simplege/components/texture.h>

#include <simplege/math.h>

namespace SimpleGE
{
  class CameraComponent;
  class SpriteSheetComponent;
  static void from_json(const nlohmann::json&, SpriteSheetComponent&);

  class SpriteSheetComponent : public TextureComponent
  {
  public:
    static constexpr auto Type = "SpriteSheet";

    struct Frame
    {
      Area frame;
      Size<int> sourceSize;
      Area uv;
    };

    struct Meta
    {
      Size<int> size;
    };

    struct Description : public TextureComponent::Description
    {
      std::string description;
    };

    SpriteSheetComponent(Entity& owner) : TextureComponent(owner), Component(owner) {}

    SetupResult Setup(const nlohmann::json& descr) override;
    SetupResult Setup(const Description& descr);

    [[nodiscard]] const Frame* GetSprite(std::string_view key) const;

    void Draw(const CameraComponent& camera, const ShaderProgram::VertexBufferType& vertexBuffer,
              const ShaderProgram::IndexBufferType& indexBuffer, std::size_t indexCount);
  private:
    void SetupImpl(const Description& descr);

    std::unordered_map<std::string, SpriteSheetComponent::Frame> sprites;
    SpriteSheetComponent::Meta meta{};

    friend void from_json(const nlohmann::json&, SpriteSheetComponent&);
  };
} // namespace SimpleGE
