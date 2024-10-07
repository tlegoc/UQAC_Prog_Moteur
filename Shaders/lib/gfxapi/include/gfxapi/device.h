#pragma once

#include <gfxapi/pch/precomp.h>

namespace SimpleGE::GFXAPI
{
  class Adapter;
  class DescriptorPool;
  class Queue;
  class Window;

  struct DeviceConfig
  {
    const Adapter& adapter;
    const Window& window;
  };

  class Device
  {
  public:
    static Device& GetMain()
    {
      Expects(mainDevice != nullptr);
      return *mainDevice;
    }

    static void SetMain(Device* device)
    {
      Expects(device != nullptr || mainDevice != nullptr);
      mainDevice = device;
    }

    static std::unique_ptr<Device> Create(const DeviceConfig& config) { return std::make_unique<Device>(config); }

    Device(const DeviceConfig& config);
    Device(const Device&) = delete;
    Device(Device&&) = delete;
    ~Device();

    Device& operator=(const Device&) = delete;
    Device& operator=(Device&&) = delete;

    [[nodiscard]] const Queue& GetGraphicsQueue() const { return *gsl::make_not_null(graphicsQueue.get()); }
    [[nodiscard]] const Queue& GetPresentQueue() const { return *gsl::make_not_null(presentQueue.get()); }
    [[nodiscard]] const DescriptorPool& GetDescriptorPool() const { return *gsl::make_not_null(descriptorPool.get()); }

    [[nodiscard]] const Adapter& GetAdapter() const { return adapter; }

    [[nodiscard]] const vk::Device& GetNative() const { return device; }

  private:
    static inline Device* mainDevice{};

    const Adapter& adapter;
    vk::Device device;

    std::unique_ptr<DescriptorPool> descriptorPool;
    std::unique_ptr<Queue> graphicsQueue;
    std::unique_ptr<Queue> presentQueue;
  };
} // namespace SimpleGE::GFXAPI
