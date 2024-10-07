#pragma once

#include <simplege/pch/precomp.h>

#include <simplege/math.h>

namespace SimpleGE
{
  static void from_json(const nlohmann::json& j, Point<2>& pos)
  {
    j.at("x").get_to(pos[0]);
    j.at("y").get_to(pos[1]);
  }

  template<typename T>
  static void from_json(const nlohmann::json& j, Size<T>& size)
  {
    j.at("w").get_to(size.width);
    j.at("h").get_to(size.height);
  }

  static void from_json(const nlohmann::json& j, Area& area)
  {
    from_json(j, area.position);
    from_json(j, area.size);
  }
} // namespace SimpleGE
