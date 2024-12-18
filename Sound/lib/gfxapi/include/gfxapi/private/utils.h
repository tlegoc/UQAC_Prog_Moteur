#pragma once

#include <gfxapi/pch/precomp.h>

namespace SimpleGE::GFXAPI
{
  class Adapter;
  class Device;

  namespace Private
  {
    [[nodiscard]] std::uint32_t FindMemoryType(const Adapter& adapter, std::uint32_t typeFilter,
                                               vk::MemoryPropertyFlags properties);
    [[nodiscard]] ColorFormat FindDepthFormat(const Device& device);
    [[nodiscard]] vk::ShaderStageFlagBits GetVkShaderType(ShaderType type);
  } // namespace Private
} // namespace SimpleGE::GFXAPI
