#pragma once

#include <gfxapi/gfxapi.h>

namespace SimpleGE::GFXAPI
{
  namespace Private
  {
    template<typename T>
    struct BufferIndexType
    {
    };

    template<>
    struct BufferIndexType<std::uint16_t>
    {
      static constexpr auto IndexType = vk::IndexType::eUint16;
    };

    template<>
    struct BufferIndexType<std::uint32_t>
    {
      static constexpr auto IndexType = vk::IndexType::eUint32;
    };
  } // namespace Private

  template<typename T>
  inline void CommandBuffer::BindIndexBuffer(const TypedBuffer<T>& buffer)
  {
    BindIndexBuffer(buffer, Private::BufferIndexType<T>::IndexType);
  }
} // namespace SimpleGE::GFXAPI
