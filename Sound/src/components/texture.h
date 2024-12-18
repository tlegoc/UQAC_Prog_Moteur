#pragma once

#include <simplege/pch/precomp.h>

namespace SimpleGE
{
  static void from_json(const nlohmann::json& j, TextureComponent::Description& desc)
  {
    j.at("texture").get_to(desc.texture);
    j.at("shaderProgram").get_to(desc.shaderProgram);
  }
} // namespace SimpleGE