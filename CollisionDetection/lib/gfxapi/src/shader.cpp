#include <gfxapi/pch/precomp.h>

#include <gfxapi/gfxapi.h>

namespace SimpleGE::GFXAPI
{
  Shader::Shader(const ShaderConfig& config) : device(config.device)
  {
    vk::ShaderModuleCreateInfo create_info{
        .codeSize = std::size(config.code),
        .pCode = std::bit_cast<const uint32_t*>(config.code.data()),
    };

    module = device.GetNative().createShaderModule(create_info);
    if (!module)
    {
      throw std::runtime_error("Impossible de créer le shader");
    }

    stage = vk::PipelineShaderStageCreateInfo{
        .stage = Private::GetVkShaderType(config.type),
        .module = module,
        .pName = config.entryPoint.data(),
    };
  }

  Shader::Shader(Shader&& other) noexcept : device(other.device), module(other.module), stage(other.stage)
  {
    other.module = nullptr;
  }

  Shader::~Shader()
  {
    if (module)
    {
      device.GetNative().destroyShaderModule(module);
    }
  }

  Shader& Shader::operator=(Shader&& other) noexcept
  {
    Expects(&device == &other.device);
    this->~Shader();
    module = other.module;
    stage = other.stage;
    other.module = nullptr;
    return *this;
  }

} // namespace SimpleGE::GFXAPI