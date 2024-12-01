#pragma once

#include <simplege/pch/precomp.h>

#include <simplege/components/texture.h>
#include <simplege/components/visual.h>

namespace SimpleGE
{
  class RawSpriteComponent : public VisualComponent, public TextureComponent
  {
  public:
    static constexpr auto Type = "RawSprite";

    struct Description : public TextureComponent::Description
    {
      float width{};
      float height{};
      float scale{1.F};
    };

    RawSpriteComponent(Entity& owner) : TextureComponent(owner), Component(owner) {}

    SetupResult Setup(const nlohmann::json& descr) override;
    SetupResult Setup(const Description& descr);

    void Display(const CameraComponent& camera, const Timing& timing) override;

  private:
    void SetupImpl(const Description& descr);
    void UpdateComponents(const Description& descr, Image& texture);

    static constexpr std::array<ShaderProgram::Index, 6> Indices{0, 1, 2, 2, 3, 0};

    std::unique_ptr<ShaderProgram::VertexBufferType> vertexBuffer;
    std::unique_ptr<ShaderProgram::IndexBufferType> indexBuffer;
  };
} // namespace SimpleGE
