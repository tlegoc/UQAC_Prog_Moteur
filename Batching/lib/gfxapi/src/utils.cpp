#include <gfxapi/pch/precomp.h>

#include <gfxapi/gfxapi.h>

namespace SimpleGE::GFXAPI::Private
{
  [[nodiscard]] static ColorFormat FindSupportedFormat(const Device& device, std::span<const ColorFormat> candidates,
                                                       vk::ImageTiling tiling, vk::FormatFeatureFlags features)
  {
    auto physicalDevice = device.GetAdapter().GetNative();

    for (const auto& format : candidates)
    {
      auto props = physicalDevice.getFormatProperties(format);
      if (tiling == vk::ImageTiling::eLinear && (props.linearTilingFeatures & features) == features)
      {
        return format;
      }
      if (tiling == vk::ImageTiling::eOptimal && (props.optimalTilingFeatures & features) == features)
      {
        return format;
      }
    }

    throw std::runtime_error("Failed to find supported format!");
  }

  std::uint32_t FindMemoryType(const Adapter& adapter, std::uint32_t typeFilter, vk::MemoryPropertyFlags properties)
  {
    auto memProperties = adapter.GetNative().getMemoryProperties();
    for (std::uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
    {
      if ((typeFilter & (1 << i)) != 0 && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
      {
        return i;
      }
    }

    throw std::runtime_error("Failed to find suitable memory type!");
  }

  ColorFormat FindDepthFormat(const Device& device)
  {
    return FindSupportedFormat(
        device, std::array{ColorFormat::eD32Sfloat, ColorFormat::eD32SfloatS8Uint, ColorFormat::eD24UnormS8Uint},
        vk::ImageTiling::eOptimal, vk::FormatFeatureFlagBits::eDepthStencilAttachment);
  }

  [[nodiscard]] vk::ShaderStageFlagBits GetVkShaderType(ShaderType type)
  {
    switch (type)
    {
    case ShaderType::Vertex:
    {
      return vk::ShaderStageFlagBits::eVertex;
    }
    case ShaderType::Fragment:
    {
      return vk::ShaderStageFlagBits::eFragment;
    }
    default:
    {
      throw std::runtime_error("Type de shader non supporté");
    }
    }
  }
} // namespace SimpleGE::GFXAPI::Private
