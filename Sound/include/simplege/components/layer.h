#pragma once

#include <simplege/pch/precomp.h>

#include <simplege/components/visual.h>

namespace SimpleGE
{
  class LayerComponent : public VisualComponent
  {
  public:
    static constexpr auto Type = "Layer";

    LayerComponent(Entity& owner) : Component(owner) {}

    void Display(const CameraComponent& camera, const Timing& timing) override;
  };
} // namespace SimpleGE
