#pragma once

#include <simplege/pch/precomp.h>

#include <simplege/components/logic.h>

namespace SimpleGE
{
  class DebugDrawCallsComponent : public LogicComponent
  {
  public:
    static constexpr auto Type = "DebugDrawCalls";

    DebugDrawCallsComponent(Entity& owner) : Component(owner) {}

    void UpdateLogic(const Timing& timing) override;
  };
} // namespace SimpleGE
