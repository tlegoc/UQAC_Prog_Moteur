#pragma once

#include <simplege/pch/precomp.h>

namespace SimpleGE
{
  struct Timing
  {
    std::chrono::milliseconds delta;
    unsigned int frame;
  };
} // namespace SimpleGE