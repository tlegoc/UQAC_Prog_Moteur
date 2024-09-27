#include <gfxapi/pch/precomp.h>

#include <gfxapi/gfxapi.h>

namespace SimpleGE::GFXAPI
{
  Buffer::Buffer(const Device& device, std::size_t elemSize, std::size_t count, vk::BufferUsageFlags usage,
                 vk::MemoryPropertyFlags properties)
      : device(device)
  {
    const auto& vkDevice = device.GetNative();

    vk::BufferCreateInfo bufferInfo{
        .flags = {},
        .size = count * elemSize,
        .usage = usage,
        .sharingMode = vk::SharingMode::eExclusive,
    };

    buffer = vkDevice.createBuffer(bufferInfo);

    vk::MemoryRequirements memRequirements = vkDevice.getBufferMemoryRequirements(buffer);

    vk::MemoryAllocateInfo allocInfo{
        .allocationSize = memRequirements.size,
        .memoryTypeIndex = Private::FindMemoryType(device.GetAdapter(), memRequirements.memoryTypeBits, properties),
    };

    bufferMemory = vkDevice.allocateMemory(allocInfo);

    vkDevice.bindBufferMemory(buffer, bufferMemory, 0);
  }

  Buffer::Buffer(Buffer&& other) noexcept : device(other.device), buffer(other.buffer), bufferMemory(other.bufferMemory)
  {
    other.buffer = nullptr;
    other.bufferMemory = nullptr;
  }

  Buffer::~Buffer()
  {
    const auto& vkDevice = device.GetNative();
    vkDevice.destroyBuffer(buffer);
    vkDevice.freeMemory(bufferMemory);
  }

  Buffer& Buffer::operator=(Buffer&& other) noexcept
  {
    Expects(&device == &other.device);
    this->~Buffer();
    buffer = other.buffer;
    bufferMemory = other.bufferMemory;
    other.buffer = nullptr;
    other.bufferMemory = nullptr;
    return *this;
  }

  void Buffer::CopyTo(Texture& target, std::uint32_t width, std::uint32_t height)
  {
    auto commandBuffer = ImmediateCommandBuffer({device.GetGraphicsQueue()});

    vk::BufferImageCopy region{.bufferOffset = 0,
                               .bufferRowLength = 0,
                               .bufferImageHeight = 0,
                               .imageSubresource =
                                   {
                                       .aspectMask = vk::ImageAspectFlagBits::eColor,
                                       .mipLevel = 0,
                                       .baseArrayLayer = 0,
                                       .layerCount = 1,
                                   },
                               .imageOffset =
                                   {
                                       .x = 0,
                                       .y = 0,
                                       .z = 0,
                                   },
                               .imageExtent = {
                                   .width = width,
                                   .height = height,
                                   .depth = 1,
                               }};

    commandBuffer.Copy(*this, target, region);
  }

  void Buffer::CopyTo(Buffer& target, std::uint32_t offset, std::uint32_t size)
  {
    auto commandBuffer = ImmediateCommandBuffer({device.GetGraphicsQueue()});

    vk::BufferCopy region{.srcOffset = 0, .dstOffset = offset, .size = size};

    commandBuffer.Copy(*this, target, region);
  }
} // namespace SimpleGE::GFXAPI