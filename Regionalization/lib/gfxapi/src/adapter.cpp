#include <gfxapi/pch/precomp.h>

#include <gfxapi/gfxapi.h>

namespace SimpleGE::GFXAPI
{
  [[nodiscard]] QueueFamilyIndices FindQueueFamilies(const vk::PhysicalDevice& device, const Window& window)
  {
    QueueFamilyIndices indices;
    const auto& surface = window.GetNativeSurface();

    int i = 0;
    for (const auto& queueFamily : device.getQueueFamilyProperties())
    {
      if (device.getSurfaceSupportKHR(i, surface) == VK_TRUE)
      {
        indices.presentFamily = i;
      }

      if (queueFamily.queueFlags & vk::QueueFlagBits::eGraphics)
      {
        indices.graphicsFamily = i;
      }

      i++;
    }

    return indices;
  }

  [[nodiscard]] SwapChainSupportDetails QuerySwapChainSupport(const vk::PhysicalDevice& device, const Window& window)
  {
    const auto& surface = window.GetNativeSurface();

    SwapChainSupportDetails details;
    details.capabilities = device.getSurfaceCapabilitiesKHR(surface);
    details.formats = device.getSurfaceFormatsKHR(surface);
    details.presentModes = device.getSurfacePresentModesKHR(surface);

    return details;
  }

  [[nodiscard]] bool CheckDeviceExtensionSupport(const vk::PhysicalDevice& device)
  {
    auto availableExtensions = device.enumerateDeviceExtensionProperties();
    for (const auto* extensionName : Adapter::DeviceExtensions)
    {
      if (!std::any_of(availableExtensions.begin(), availableExtensions.end(),
                       [extensionName](const auto& e) { return std::strcmp(extensionName, e.extensionName); }))
      {
        return false;
      }
    }

    return true;
  }

  int Adapter::RateDeviceSuitability(const vk::PhysicalDevice& device, const Window& window)
  {
    constexpr auto Best = 1000;
    constexpr auto Worst = 1;
    constexpr auto Invalid = -1;

    auto indices = GFXAPI::FindQueueFamilies(device, window);
    bool extensionsSupported = CheckDeviceExtensionSupport(device);

    bool swapChainAdequate = false;
    if (extensionsSupported)
    {
      SwapChainSupportDetails swapChainSupport = GFXAPI::QuerySwapChainSupport(device, window);
      swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
    }

    if (!indices.IsComplete() || !extensionsSupported || !swapChainAdequate)
    {
      return Invalid;
    }

    auto properties = device.getProperties();

    if (properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu)
    {
      return Best;
    }

    return Worst;
  }

  Adapter::Adapter(const AdapterConfig& config)
      : instance(config.instance), physicalDevice(config.adapterInfos.physicalDevice)
  {
  }

  Adapter::Adapter(Adapter&& other) noexcept : instance(other.instance), physicalDevice(other.physicalDevice)
  {
    other.physicalDevice = nullptr;
  }

  Adapter& Adapter::operator=(Adapter&& other) noexcept
  {
    Expects(&instance == &other.instance);
    physicalDevice = other.physicalDevice;
    other.physicalDevice = nullptr;
    return *this;
  }

  QueueFamilyIndices Adapter::FindQueueFamilies(const Window& window) const
  {
    return GFXAPI::FindQueueFamilies(physicalDevice, window);
  }

  SwapChainSupportDetails Adapter::QuerySwapChainSupport(const Window& window) const
  {
    return GFXAPI::QuerySwapChainSupport(physicalDevice, window);
  }

} // namespace SimpleGE::GFXAPI