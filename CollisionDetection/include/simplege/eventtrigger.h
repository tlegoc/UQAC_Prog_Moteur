#pragma once

#include <simplege/pch/precomp.h>

namespace SimpleGE
{
  template<typename T>
  struct EventTrigger
  {
    void Register(const T& fn) { handler.push_back(fn); }

    template<typename... Args>
    void Trigger(Args&... args)
    {
      for (auto& fn : handler)
      {
        fn(args...);
      }
    }

    std::vector<T> handler;
  };
} // namespace SimpleGE