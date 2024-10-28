#pragma once

#include <gfxapi/pch/precomp.h>

namespace SimpleGE::GFXAPI
{
  class Buffer;

  template<typename T>
  struct MemoryMapSettings
  {
    std::ptrdiff_t offset{};
    std::size_t count{};
    vk::MemoryMapFlags flags{};
  };

  template<typename T>
  struct MemoryMapConfig
  {
    const Buffer& buffer;
    const MemoryMapSettings<T>& settings;
  };

  template<typename T>
  class MemoryMap
  {
  public:
    inline MemoryMap(const MemoryMapConfig<T>& config);
    MemoryMap(const MemoryMap<T>&) = delete;
    inline MemoryMap(MemoryMap<T>&& other) noexcept;
    inline ~MemoryMap();

    MemoryMap<T>& operator=(const MemoryMap<T>&) = delete;
    inline MemoryMap<T>& operator=(MemoryMap<T>&& other) noexcept;

    std::span<T> Get() { return std::span<T>(GetImpl(), count); }
    std::span<const T> Get() const { return std::span<const T>(GetImpl(), count); }

    operator std::span<T>() { return Get(); }

    operator std::span<const T>() const { return Get(); }

  private:
    T* GetImpl() const
    {
      Expects(data);
      return const_cast<T*>(data);
    }

    const Buffer& buffer;
    std::size_t count{};
    T* data{};
  };
} // namespace SimpleGE::GFXAPI
