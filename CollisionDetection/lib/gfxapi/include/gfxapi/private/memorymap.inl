#pragma once

#include <gfxapi/gfxapi.h>

namespace SimpleGE::GFXAPI
{
  template<typename T>
  inline MemoryMap<T>::MemoryMap(const MemoryMapConfig<T>& config) : buffer(config.buffer), count(config.settings.count)
  {
    const auto& vkDevice = buffer.GetDevice().GetNative();
    vk::DeviceSize size = config.settings.count * sizeof(T);
    data = static_cast<T*>(
        vkDevice.mapMemory(buffer.GetNativeMemory(), config.settings.offset, size, config.settings.flags));
  }

  template<typename T>
  inline MemoryMap<T>::MemoryMap(MemoryMap<T>&& other) noexcept
      : buffer(other.buffer), count(other.count), data(other.data)
  {
    other.data = nullptr;
  }

  template<typename T>
  inline MemoryMap<T>::~MemoryMap()
  {
    if (data != nullptr)
    {
      const auto& vkDevice = buffer.GetDevice().GetNative();
      vkDevice.unmapMemory(buffer.GetNativeMemory());
    }
  }

  template<typename T>
  inline MemoryMap<T>& MemoryMap<T>::operator=(MemoryMap<T>&& other) noexcept
  {
    Expects(&buffer == &other.buffer);
    this->~MemoryMap();
    count = other.count;
    data = other.data;
    other.data = nullptr;
    return *this;
  }

} // namespace SimpleGE::GFXAPI