#include <simplege/pch/precomp.h>

#include <simplege/simplege.h>

namespace SimpleGE::Graphic
{
  void BaseShaderProgram::Register(std::string_view name, std::unique_ptr<BaseShaderProgram> shaderProgram)
  {
    shaderProgram->Init();
    shaderProgramRegistry.emplace(name, std::move(shaderProgram));
  }

  const BaseShaderProgram& BaseShaderProgram::GetShaderProgram(std::string_view name)
  {
    Expects(shaderProgramRegistry.find((std::string) name) != shaderProgramRegistry.end());
    gsl::not_null<BaseShaderProgram*> shaderProgram = shaderProgramRegistry.at((std::string) name).get();
    return *shaderProgram;
  }

  BaseShaderProgram::BaseShaderProgram(const Config& config)
      : vertexInputInfo(config.vertexInputInfo), bindingCount(config.bindingCount)
  {
    auto vsData = Resources::Get<BinAsset>(config.vertexShader);
    Ensures(vsData != nullptr);

    auto fsData = Resources::Get<BinAsset>(config.fragmentShader);
    Ensures(fsData != nullptr);

    shaders.emplace_back(GFXAPI::Shader({
        .type = GFXAPI::ShaderType::Vertex,
        .code = vsData->Value(),
    }));

    shaders.emplace_back(GFXAPI::Shader({
        .type = GFXAPI::ShaderType::Fragment,
        .code = fsData->Value(),
    }));
  }

  void BaseShaderProgram::Init()
  {
    Expects(bindingCount == uniformInfo.CountBindings());
    descriptorSetLayout = GFXAPI::DescriptorSetLayout::Create({.uniformInfo = uniformInfo});
  }
} // namespace SimpleGE::Graphic
