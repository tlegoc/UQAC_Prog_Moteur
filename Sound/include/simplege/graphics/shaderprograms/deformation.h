#pragma once

#include <simplege/pch/precomp.h>

#include <simplege/graphics/shaderprograms/shaderprogram.h>

namespace SimpleGE::Graphic
{
  class DeformationShaderProgram;

  template<>
  struct ShaderProgramTrait<DeformationShaderProgram>
  {
    static constexpr auto Name = "deformation";
    static constexpr auto VertexShader = "shaders/deformation.vert.spv";
    static constexpr auto FragmentShader = "shaders/deformation.frag.spv";

    enum class Binding
    {
      Texture = 0,
      Deformation,
      Intensity,
      Settings,

      Count
    };

    using Index = std::uint16_t;

    struct Vertex
    {
    };

    struct UniformBufferObject
    {
      glm::float32 time;
      glm::float32 scale;
    };

    static GFXAPI::VertexInputInfo GetVertexInputInfo() { return {}; }
  };

  class DeformationShaderProgram : public ShaderProgram<DeformationShaderProgram>
  {
  protected:
    DeformationShaderProgram()
    {
      AddTextureBinding(Binding::Texture, GFXAPI::ShaderType::Fragment);
      AddTextureBinding(Binding::Deformation, GFXAPI::ShaderType::Fragment);
      AddTextureBinding(Binding::Intensity, GFXAPI::ShaderType::Fragment);
      AddUniformBinding(Binding::Settings, GFXAPI::ShaderType::Fragment);
    }

  private:
    friend std::unique_ptr<DeformationShaderProgram> std::make_unique<DeformationShaderProgram>();
  };
} // namespace SimpleGE::Graphic