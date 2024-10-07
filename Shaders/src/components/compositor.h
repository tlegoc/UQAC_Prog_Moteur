#pragma once

#include <simplege/pch/precomp.h>

#include <simplege/components/compositor.h>

namespace SimpleGE
{
  static void from_json(const nlohmann::json& j, CompositorComponent::Description& desc)
  {
    j.at("shaderProgram").get_to(desc.shaderProgram);
  }
} // namespace SimpleGE
