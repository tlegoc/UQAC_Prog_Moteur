#include <simplege/pch/precomp.h>

#include <simplege/simplege.h>

namespace SimpleGE
{
  static std::unordered_map<std::string, CommandLine::CallbackFn> g_commandLineOptions;

  void CommandLine::RegisterOption(std::string_view key, const CallbackFn& func)
  {
    g_commandLineOptions.emplace(key, func);
  }

  void CommandLine::Parse(std::span<char*> args)
  {
    for (int i = 1; i < std::size(args); i++)
    {
      const auto* curArg = gsl::at(args, i);

      auto cmdOpt = g_commandLineOptions.find(curArg);
      if (cmdOpt == g_commandLineOptions.end())
      {
        fmt::print(stderr, "Option \"{}\" inconnue\n", curArg);
        continue;
      }

      i++;
      cmdOpt->second(gsl::at(args, i));
    }
  }
} // namespace SimpleGE
