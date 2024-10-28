#pragma once

#include <simplege/pch/precomp.h>

#include <simplege/graphics/shaderprograms/shaderprogram.h>

namespace SimpleGE::Graphic
{
  class RenderQuadShaderProgram;

  template<>
  struct ShaderProgramTrait<RenderQuadShaderProgram>
  {
    static constexpr auto Name = "renderquad";
    static constexpr auto VertexShader = "shaders/renderquad.vert.spv";
    static constexpr auto FragmentShader = "shaders/renderquad.frag.spv";

    enum class Binding
    {
      Texture = 0,

      Count
    };

    using Index = std::uint16_t;

    struct Vertex
    {
    };

    struct UniformBufferObject
    {
    };

    static GFXAPI::VertexInputInfo GetVertexInputInfo() { return {}; }
  };

  class RenderQuadShaderProgram : public ShaderProgram<RenderQuadShaderProgram>
  {
  protected:
    RenderQuadShaderProgram() { AddTextureBinding(Binding::Texture, GFXAPI::ShaderType::Fragment); }

  private:
    friend std::unique_ptr<RenderQuadShaderProgram> std::make_unique<RenderQuadShaderProgram>();
  };
} // namespace SimpleGE::Graphic