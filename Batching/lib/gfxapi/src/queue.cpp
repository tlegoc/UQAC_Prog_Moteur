#include <gfxapi/pch/precomp.h>

#include <gfxapi/gfxapi.h>

namespace SimpleGE::GFXAPI
{
  Queue::Queue(const QueueConfig& config) : device(config.device)
  {
    const auto& vkDevice = device.GetNative();

    queue = vkDevice.getQueue(config.family, 0);

    vk::CommandPoolCreateInfo poolInfo{
        .flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
        .queueFamilyIndex = config.family,
    };

    commandPool = vkDevice.createCommandPool(poolInfo);
  }

  Queue::Queue(Queue&& other) noexcept : device(other.device), queue(other.queue), commandPool(other.commandPool)
  {
    other.queue = nullptr;
    other.commandPool = nullptr;
  }

  Queue::~Queue()
  {
    const auto& vkDevice = device.GetNative();

    vkDevice.destroyCommandPool(commandPool);
  }

  Queue& Queue::operator=(Queue&& other) noexcept
  {
    Expects(&device == &other.device);
    this->~Queue();
    queue = other.queue;
    commandPool = other.commandPool;
    other.queue = nullptr;
    other.commandPool = nullptr;
    return *this;
  }

  void Queue::Submit(const CommandBuffer& commandBuffer) const
  {
    vk::SubmitInfo submitInfo{
        .commandBufferCount = 1,
        .pCommandBuffers = &commandBuffer.GetNative(),
    };

    queue.submit({submitInfo});
  }

  void Queue::WaitIdle() const { queue.waitIdle(); }
} // namespace SimpleGE::GFXAPI
