#pragma once

#include <simplege/pch/precomp.h>

namespace SimpleGE
{
  class Image
  {
  public:
    static ImageRef Create(const std::vector<std::byte>& content) { return std::make_shared<Image>(content); }

    explicit Image(const std::vector<std::byte>& content);

    [[nodiscard]] GFXAPI::Size<std::uint32_t> Size() const { return size; }
    [[nodiscard]] std::span<const GFXAPI::ColorU8> Pixels() const { return m_Content; }
    operator std::span<const GFXAPI::ColorU8>() const { return Pixels(); }

  private:
    GFXAPI::Size<std::uint32_t> size{};
    std::vector<GFXAPI::ColorU8> m_Content;
  };
} // namespace SimpleGE