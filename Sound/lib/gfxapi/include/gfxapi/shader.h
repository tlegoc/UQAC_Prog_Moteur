#pragma once

#include <gfxapi/pch/precomp.h>

#include <gfxapi/device.h>

namespace SimpleGE::GFXAPI
{
  struct ShaderConfig
  {
    const Device& device{Device::GetMain()};
    ShaderType type;
    std::span<const std::byte> code;
    std::string_view entryPoint{"main"};
  };

  class Shader
  {
  public:
    static std::unique_ptr<Shader> Create(const ShaderConfig& config) { return std::make_unique<Shader>(config); }

    Shader(const ShaderConfig& config);
    Shader(const Shader&) = delete;
    Shader(Shader&& other) noexcept;
    ~Shader();

    Shader& operator=(const Shader&) = delete;
    Shader& operator=(Shader&& other) noexcept;

    [[nodiscard]] const Device& GetDevice() const { return device; }
    [[nodiscard]] const vk::PipelineShaderStageCreateInfo& GetNativeStage() const { return stage; }

  private:
    const Device& device;
    vk::ShaderModule module;
    vk::PipelineShaderStageCreateInfo stage;
  };
} // namespace SimpleGE::GFXAPI