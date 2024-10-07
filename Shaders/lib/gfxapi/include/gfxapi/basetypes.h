#pragma once

#include <gfxapi/pch/precomp.h>

namespace SimpleGE::GFXAPI
{
  using ColorFormat = vk::Format;

  template<typename T>
  using Color = std::array<T, 4>;

  using ColorU8 = Color<std::uint8_t>;
  using ColorFloat = Color<float>;

  enum class ShaderType
  {
    Vertex,
    Fragment,
  };

  template<typename T>
  struct Size
  {
    T width;
    T height;

    operator vk::Extent2D()
    {
      return vk::Extent2D{.width = static_cast<std::uint32_t>(width), .height = static_cast<std::uint32_t>(height)};
    }
  };
} // namespace SimpleGE::GFXAPI