#pragma once

#include <simplege/pch/precomp.h>

#include <simplege/binasset.h>
#include <simplege/image.h>
#include <simplege/textasset.h>

namespace SimpleGE::Resources
{
  constexpr auto DataPath = "data";

  template<typename T>
  [[nodiscard]] std::shared_ptr<T> Get(std::string_view name);

  template<>
  [[nodiscard]] BinAssetRef Get<BinAsset>(std::string_view name);

  template<>
  [[nodiscard]] ImageRef Get<Image>(std::string_view name);

  template<>
  [[nodiscard]] TextAssetRef Get<TextAsset>(std::string_view name);
} // namespace SimpleGE::Resources