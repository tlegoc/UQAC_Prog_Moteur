#pragma once

#include <simplege/pch/precomp.h>

#include <simplege/components/component.h>
#include <simplege/graphics/shaderprograms/sprite.h>

#include <simplege/image.h>

namespace SimpleGE
{
  namespace Graphic
  {
    class Material;
  }

  class Image;

  class TextureComponent : public virtual Component
  {
  public:
    using ShaderProgram = Graphic::SpriteShaderProgram;

    struct Description
    {
      std::string texture;
      std::string shaderProgram;
    };

    TextureComponent(Entity& owner) : Component(owner) {}

    SetupResult Setup(const nlohmann::json& descr) override;
    SetupResult Setup(const Description& descr);

    [[nodiscard]] ImageRef GetTexture() const { return image; }
    [[nodiscard]] ShaderProgram::UniformBufferObject& GetUniformBuffer()
    {
      return *gsl::make_not_null(uniformBuffer.get());
    }

  protected:
    ImageRef image;
    std::unique_ptr<GFXAPI::Texture> textureImage;
    std::unique_ptr<GFXAPI::Sampler> textureSampler;
    std::unique_ptr<GFXAPI::Pipeline> pipeline;
    std::unique_ptr<ShaderProgram::UniformBufferType> uniformBuffer;
    std::unique_ptr<Graphic::Material> material;

  private:
    void SetupImpl(const Description& descr);
  };
} // namespace SimpleGE
