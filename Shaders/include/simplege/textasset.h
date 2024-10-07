#pragma once

#include <simplege/pch/precomp.h>

namespace SimpleGE
{
  class TextAsset
  {
  public:
    static TextAssetRef Create(std::string&& content) { return std::make_shared<TextAsset>(std::move(content)); }

    explicit TextAsset(std::string&& content) : m_Content(std::move(content)) {}

    [[nodiscard]] std::string_view Value() const { return m_Content; }

    operator std::string_view() const { return Value(); }

  private:
    std::string m_Content;

    friend TextAssetRef std::make_shared<TextAsset>();
  };
} // namespace SimpleGE