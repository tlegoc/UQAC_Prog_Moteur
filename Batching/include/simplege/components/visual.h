#pragma once

#include <simplege/pch/precomp.h>

#include <simplege/components/component.h>
#include <simplege/systems/displaysystem.h>
#include <simplege/timing.h>

namespace SimpleGE
{
  class CameraComponent;

  class VisualComponent : public virtual Component
  {
  public:
    VisualComponent() { DisplaySystem::Instance().Register(this); }

    VisualComponent(const VisualComponent&) = default;
    VisualComponent(VisualComponent&&) = default;
    VisualComponent& operator=(const VisualComponent&) = delete;
    VisualComponent& operator=(VisualComponent&&) = delete;

    ~VisualComponent() override { DisplaySystem::Instance().Unregister(this); }

    virtual void Display(const CameraComponent& camera, const Timing& timing) = 0;
  };
} // namespace SimpleGE
