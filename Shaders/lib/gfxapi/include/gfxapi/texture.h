#pragma once

#include <gfxapi/pch/precomp.h>

#include <gfxapi/device.h>
#include <gfxapi/private/utils.h>

namespace SimpleGE::GFXAPI
{
  class Device;

  struct TextureConfig
  {
    const Device& device{Device::GetMain()};
    Size<std::uint32_t> size;
    ColorFormat format{ColorFormat::eR8G8B8A8Unorm};
    vk::ImageTiling tiling{vk::ImageTiling::eOptimal};
    vk::ImageUsageFlags usage{vk::ImageUsageFlagBits::eSampled};
    vk::MemoryPropertyFlags properties{vk::MemoryPropertyFlagBits::eDeviceLocal};
    vk::ImageAspectFlags aspectFlags{vk::ImageAspectFlagBits::eColor};
  };

  struct DepthTextureConfig
  {
    const Device& device{Device::GetMain()};
    Size<std::uint32_t> size;
    ColorFormat format{Private::FindDepthFormat(device)};
    vk::ImageTiling tiling{vk::ImageTiling::eOptimal};
    vk::ImageUsageFlags usage{vk::ImageUsageFlagBits::eDepthStencilAttachment};
    vk::MemoryPropertyFlags properties{vk::MemoryPropertyFlagBits::eDeviceLocal};
    vk::ImageAspectFlags aspectFlags{vk::ImageAspectFlagBits::eDepth};

    operator TextureConfig() noexcept
    {
      return TextureConfig{
          .device = device,
          .size = size,
          .format = format,
          .tiling = tiling,
          .usage = usage,
          .properties = properties,
          .aspectFlags = aspectFlags,
      };
    }
  };

  struct SwapTextureConfig
  {
    const Device& device;
    Size<std::uint32_t> size;
    vk::Image image;
    ColorFormat format;
    vk::ImageAspectFlags aspectFlags;
  };

  class Texture
  {
  public:
    static std::unique_ptr<Texture> Create(const TextureConfig& config) { return std::make_unique<Texture>(config); }

    static std::unique_ptr<Texture> Create(TextureConfig config, const std::span<const ColorU8>& pixels);

    Texture(const TextureConfig& config);
    Texture(const SwapTextureConfig& config);

    Texture(const Texture&) = delete;
    Texture(Texture&& other) noexcept;
    ~Texture() { Destroy(); }

    Texture& operator=(const Texture&) = delete;
    Texture& operator=(Texture&& other) noexcept;

    [[nodiscard]] ColorFormat GetColorFormat() const { return format; }
    [[nodiscard]] Size<std::uint32_t> GetSize() const { return size; }

    [[nodiscard]] const vk::Image& GetNative() const { return image; }
    [[nodiscard]] const vk::ImageView& GetNativeView() const { return imageView; }

  private:
    enum class Type
    {
      Standard,
      SwapChain,
    };

    void Destroy();

    void CreateImageView(vk::ImageAspectFlags aspectFlags);

    void Transition(vk::ImageLayout oldLayout, vk::ImageLayout newLayout);

    const Device& device;
    Type type;
    vk::Image image;
    vk::ImageView imageView;
    vk::DeviceMemory imageMemory;
    ColorFormat format;
    Size<std::uint32_t> size;
  };

  struct SamplerConfig
  {
    const Device& device{Device::GetMain()};
    bool clamp{true};
  };

  class Sampler
  {
  public:
    static std::unique_ptr<Sampler> Create(const SamplerConfig& config) { return std::make_unique<Sampler>(config); }

    Sampler(const SamplerConfig& config);
    Sampler(const Sampler&) = delete;
    Sampler(Sampler&& other) noexcept;
    ~Sampler();

    Sampler& operator=(const Sampler&) = delete;
    Sampler& operator=(Sampler&& other) noexcept;

    [[nodiscard]] const vk::Sampler& GetNative() const { return sampler; }

  private:
    const Device& device;
    vk::Sampler sampler;
  };
} // namespace SimpleGE::GFXAPI
