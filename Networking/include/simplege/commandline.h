#pragma once

#include <simplege/pch/precomp.h>

namespace SimpleGE
{
  class CommandLine
  {
  public:
    using CallbackFn = std::function<void(std::string_view)>;

    static void RegisterOption(std::string_view key, const CallbackFn& func);
    static void Parse(std::span<char*> args);
  };
} // namespace SimpleGE