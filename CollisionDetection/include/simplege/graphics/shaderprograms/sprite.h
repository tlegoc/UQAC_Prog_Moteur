#pragma once

#include <simplege/pch/precomp.h>

#include <simplege/graphics/shaderprograms/shaderprogram.h>

namespace SimpleGE::Graphic
{
  class SpriteShaderProgram;

  template<>
  struct ShaderProgramTrait<SpriteShaderProgram>
  {
    static constexpr auto Name = "sprite";
    static constexpr auto VertexShader = "shaders/sprite.vert.spv";
    static constexpr auto FragmentShader = "shaders/sprite.frag.spv";

    enum class Binding : std::uint32_t
    {
      MVP = 0,
      Texture,

      Count
    };

    using Index = std::uint16_t;

    struct Vertex
    {
      alignas(16) glm::vec3 pos;
      alignas(8) glm::vec2 texCoord;
    };

    struct UniformBufferObject
    {
      alignas(16) glm::mat4 model;
      alignas(16) glm::mat4 view;
      alignas(16) glm::mat4 proj;
    };

    static GFXAPI::VertexInputInfo GetVertexInputInfo()
    {
      GFXAPI::VertexBinding<Vertex> vertexBinding;
      vertexBinding.AddAttribute({
          .format = GFXAPI::ColorFormat::eR32G32B32Sfloat,
          .offset = offsetof(Vertex, pos),
      });
      vertexBinding.AddAttribute({
          .format = GFXAPI::ColorFormat::eR32G32Sfloat,
          .offset = offsetof(Vertex, texCoord),
      });

      GFXAPI::VertexInputInfo vertexInputInfo;
      vertexInputInfo.AddBinding(vertexBinding);

      return vertexInputInfo;
    }
  };

  class SpriteShaderProgram : public ShaderProgram<SpriteShaderProgram>
  {
  protected:
    SpriteShaderProgram()
    {
      AddUniformBinding(Binding::MVP, GFXAPI::ShaderType::Vertex);
      AddTextureBinding(Binding::Texture, GFXAPI::ShaderType::Fragment);
    }

  private:
    friend std::unique_ptr<SpriteShaderProgram> std::make_unique<SpriteShaderProgram>();
  };
} // namespace SimpleGE::Graphic