#pragma once

#include <gfxapi/pch/precomp.h>

#include <gfxapi/device.h>
#include <gfxapi/memorymap.h>

namespace SimpleGE::GFXAPI
{
  class Texture;

  template<typename T = std::byte>
  struct BufferConfig
  {
    const Device& device{Device::GetMain()};
    std::size_t elemSize{sizeof(T)};
    std::size_t count{1};
    vk::BufferUsageFlags usage;
    vk::MemoryPropertyFlags properties;
  };

  class Buffer
  {
  public:
    template<typename T>
    Buffer(const BufferConfig<T>& config)
        : Buffer(config.device, config.elemSize, config.count, config.usage, config.properties)
    {
    }

    Buffer(const Buffer&) = delete;
    Buffer(Buffer&& other) noexcept;

    ~Buffer();

    Buffer& operator=(const Buffer&) = delete;
    Buffer& operator=(Buffer&& other) noexcept;

    template<typename T>
    [[nodiscard]] MemoryMap<T> Map(const MemoryMapSettings<T>& settings)
    {
      return MemoryMap<T>({*this, settings});
    }

    void CopyTo(Texture& target, std::uint32_t width, std::uint32_t height);
    void CopyTo(Buffer& target, std::uint32_t offset, std::uint32_t size);

    [[nodiscard]] const Device& GetDevice() const { return device; }
    [[nodiscard]] const vk::Buffer& GetNative() const { return buffer; }
    [[nodiscard]] const vk::DeviceMemory& GetNativeMemory() const { return bufferMemory; }

  private:
    Buffer(const Device& device, std::size_t elemSize, std::size_t count, vk::BufferUsageFlags usage,
           vk::MemoryPropertyFlags properties);

    const Device& device;
    vk::Buffer buffer;
    vk::DeviceMemory bufferMemory;
  };

  template<typename T>
  class TypedBuffer : public Buffer
  {
  public:
    static std::unique_ptr<TypedBuffer<T>> Create(const BufferConfig<T>& config)
    {
      return std::make_unique<TypedBuffer<T>>(config);
    }

    TypedBuffer(const BufferConfig<T>& config) : Buffer(config) {}
    TypedBuffer(const TypedBuffer&) = delete;
    TypedBuffer(TypedBuffer&& other) noexcept : Buffer(std::move(other)) {}
    TypedBuffer& operator=(const TypedBuffer&) = delete;

    TypedBuffer& operator=(TypedBuffer&& other) noexcept
    {
      Buffer::operator=(std::move(other));
      return *this;
    }

    MemoryMap<T> Map(const MemoryMapSettings<T>& settings) { return Buffer::Map<T>(settings); }
  };

  template<typename T>
  class UniformBuffer : public TypedBuffer<T>
  {
  public:
    using Super = TypedBuffer<T>;

    static std::unique_ptr<UniformBuffer<T>> Create(const BufferConfig<T>& config)
    {
      return std::make_unique<UniformBuffer<T>>(config);
    }

    UniformBuffer(const BufferConfig<T>& config) : Super(config), mappedBuffer(Super::Map({0, 1, {}})) {}
    UniformBuffer(const UniformBuffer&) = delete;
    UniformBuffer(UniformBuffer&& other) noexcept : Super(std::move(other)), mappedBuffer(std::move(other.mappedBuffer))
    {
    }
    UniformBuffer& operator=(const UniformBuffer&) = delete;
    UniformBuffer& operator=(UniformBuffer&& other) noexcept
    {
      Super::operator=(std::move(other));
      mappedBuffer = std::move(other.mappedBuffer);
      return *this;
    }

    T& Get() { return mappedBuffer.Get().front(); }

    operator T&() { return Get(); }

    operator const T&() const { return Get(); }

  private:
    MemoryMap<T> mappedBuffer;
  };
} // namespace SimpleGE::GFXAPI
