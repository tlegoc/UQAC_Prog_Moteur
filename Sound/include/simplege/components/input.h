#pragma once

#include <simplege/pch/precomp.h>

#include <simplege/components/component.h>

namespace SimpleGE
{
  class InputComponent : public virtual Component
  {
  public:
    enum class Action
    {
      Up,
      Down,
      Left,
      Right,
      Attack,

      Count
    };

    [[nodiscard]] virtual bool IsActive(Action action) const = 0;
  };
} // namespace SimpleGE
