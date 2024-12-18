#include <gfxapi/pch/precomp.h>

#include <gfxapi/gfxapi.h>

namespace SimpleGE::GFXAPI
{
  Device::Device(const DeviceConfig& config) : adapter(config.adapter)
  {
    QueueFamilyIndices indices = adapter.FindQueueFamilies(config.window);
    Expects(indices.IsComplete());

    std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
    std::set<std::uint32_t> uniqueQueueFamilies = {indices.GetGraphicsFamily(), indices.GetPresentFamily()};

    float queuePriority = 1.F;

    for (auto queueFamily : uniqueQueueFamilies)
    {
      vk::DeviceQueueCreateInfo queueCreateInfo{
          .queueFamilyIndex = queueFamily,
          .queueCount = 1,
          .pQueuePriorities = &queuePriority,
      };
      queueCreateInfos.push_back(queueCreateInfo);
    }

    vk::PhysicalDeviceFeatures deviceFeatures{
        // .samplerAnisotropy = VK_TRUE,
    };

    vk::DeviceCreateInfo createInfo{
        .queueCreateInfoCount = static_cast<uint32_t>(std::size(queueCreateInfos)),
        .pQueueCreateInfos = queueCreateInfos.data(),
        .enabledExtensionCount = std::size(Adapter::DeviceExtensions),
        .ppEnabledExtensionNames = Adapter::DeviceExtensions.data(),
        .pEnabledFeatures = &deviceFeatures,
    };

    if (adapter.GetGFXInstance().IsValidationEnabled())
    {
      createInfo.enabledLayerCount = std::size(ValidationLayers);
      createInfo.ppEnabledLayerNames = ValidationLayers.data();
    }
    else
    {
      createInfo.enabledLayerCount = 0;
    }

    device = adapter.GetNative().createDevice(createInfo);

    graphicsQueue = Queue::Create({*this, indices.GetGraphicsFamily()});
    presentQueue = Queue::Create({*this, indices.GetPresentFamily()});
    descriptorPool = DescriptorPool::Create({*this});
  }

  Device::~Device()
  {
    descriptorPool.reset();
    presentQueue.reset();
    graphicsQueue.reset();
    device.destroy();
  }
} // namespace SimpleGE::GFXAPI