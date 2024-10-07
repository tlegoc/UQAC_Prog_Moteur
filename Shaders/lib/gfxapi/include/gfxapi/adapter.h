#pragma once

#include <gfxapi/pch/precomp.h>

namespace SimpleGE::GFXAPI
{
  class GFXInstance;
  class Window;

  struct AdapterInfos;

  struct QueueFamilyIndices
  {
    std::optional<std::uint32_t> graphicsFamily;
    std::optional<std::uint32_t> presentFamily;

    [[nodiscard]] bool IsComplete() const { return graphicsFamily.has_value() && presentFamily.has_value(); }
    [[nodiscard]] std::uint32_t GetGraphicsFamily() const
    {
      Expects(graphicsFamily.has_value());
      return graphicsFamily.value();
    }
    [[nodiscard]] std::uint32_t GetPresentFamily() const
    {
      Expects(presentFamily.has_value());
      return presentFamily.value();
    }
  };

  struct SwapChainSupportDetails
  {
    vk::SurfaceCapabilitiesKHR capabilities;
    std::vector<vk::SurfaceFormatKHR> formats;
    std::vector<vk::PresentModeKHR> presentModes;
  };

  struct AdapterConfig
  {
    const GFXInstance& instance;
    const AdapterInfos& adapterInfos;
  };

  class Adapter
  {
  public:
    static constexpr auto DeviceExtensions = std::array{VK_KHR_SWAPCHAIN_EXTENSION_NAME};

    [[nodiscard]] static int RateDeviceSuitability(const vk::PhysicalDevice& device, const Window& window);

    static std::unique_ptr<Adapter> Create(const AdapterConfig& config) { return std::make_unique<Adapter>(config); }

    Adapter(const AdapterConfig& config);
    Adapter(const Adapter&) = delete;
    Adapter(Adapter&& other) noexcept;
    Adapter& operator=(const Adapter&) = delete;
    Adapter& operator=(Adapter&& other) noexcept;

    [[nodiscard]] QueueFamilyIndices FindQueueFamilies(const Window& window) const;
    [[nodiscard]] SwapChainSupportDetails QuerySwapChainSupport(const Window& window) const;

    [[nodiscard]] const GFXInstance& GetGFXInstance() const { return instance; }

    [[nodiscard]] const vk::PhysicalDevice& GetNative() const { return physicalDevice; }

  private:
    const GFXInstance& instance;
    vk::PhysicalDevice physicalDevice;
  };
} // namespace SimpleGE::GFXAPI
