#pragma once

#include <simplege/pch/precomp.h>

namespace SimpleGE
{
  class BinAsset
  {
  public:
    static BinAssetRef Create(std::vector<std::byte>&& content)
    {
      return std::make_shared<BinAsset>(std::move(content));
    }

    explicit BinAsset(std::vector<std::byte>&& content) : m_Content(std::move(content)) {}

    [[nodiscard]] std::span<const std::byte> Value() const { return m_Content; }

    operator std::span<const std::byte>() const { return Value(); }

  private:
    std::vector<std::byte> m_Content;

    friend BinAssetRef std::make_shared<BinAsset>();
  };
} // namespace SimpleGE