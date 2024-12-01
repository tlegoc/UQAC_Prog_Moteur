#pragma once

#include <simplege/pch/precomp.h>

#include <simplege/components/component.h>
#include <simplege/timing.h>

namespace SimpleGE
{
  class LogicComponent : public virtual Component
  {
  public:
    inline LogicComponent();
    inline ~LogicComponent() override;

    virtual void UpdateLogic(const Timing& timing) = 0;
  };
} // namespace SimpleGE
