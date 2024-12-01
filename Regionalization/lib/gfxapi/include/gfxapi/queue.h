#pragma once

#include <gfxapi/pch/precomp.h>

namespace SimpleGE::GFXAPI
{
  class CommandBuffer;
  class Device;

  struct QueueConfig
  {
    const Device& device;
    std::uint32_t family;
  };

  class Queue
  {
  public:
    static std::unique_ptr<Queue> Create(const QueueConfig& config) { return std::make_unique<Queue>(config); }

    Queue(const QueueConfig& config);
    Queue(const Queue&) = delete;
    Queue(Queue&& other) noexcept;
    ~Queue();

    Queue& operator=(const Queue&) = delete;
    Queue& operator=(Queue&& other) noexcept;

    void Submit(const CommandBuffer& commandBuffer) const;
    void WaitIdle() const;

    [[nodiscard]] const Device& GetDevice() const { return device; }
    [[nodiscard]] const vk::Queue& GetNative() const { return queue; }
    [[nodiscard]] const vk::CommandPool& GetNativeCommandPool() const { return commandPool; }

  private:
    const Device& device;
    vk::Queue queue;
    vk::CommandPool commandPool;
  };
} // namespace SimpleGE::GFXAPI
