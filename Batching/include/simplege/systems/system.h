#pragma once

#include <simplege/pch/precomp.h>

#include <simplege/timing.h>

namespace SimpleGE
{
  class ISystem
  {
  public:
    ISystem() = default;
    ISystem(const ISystem&) = default;
    ISystem(ISystem&&) = default;
    ISystem& operator=(const ISystem&) = default;
    ISystem& operator=(ISystem&&) = default;

    virtual ~ISystem() = default;

    virtual void Iterate(const Timing& timing) {}
  };
} // namespace SimpleGE