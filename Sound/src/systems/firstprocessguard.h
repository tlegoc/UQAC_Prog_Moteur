#pragma once

#ifndef _WIN32
#include <filesystem>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#endif

namespace SimpleGE
{
  struct FirstProcessGuard
  {
  public:
    static bool IsFirstProcessInstance() { return IsFirstProcessInstanceImpl(); }

  private:
#ifdef _WIN32
    static constexpr auto MutexName = "simplege";

    static FirstProcessGuard& Instance()
    {
      static FirstProcessGuard instance;
      return instance;
    }

    static bool IsFirstProcessInstanceImpl() { return Instance().isFirstInstance; }

    FirstProcessGuard() : appMutex(::CreateMutex(nullptr, TRUE, TEXT(MutexName)))
    {
      Expects(appMutex != nullptr);
      isFirstInstance = (GetLastError() != ERROR_ALREADY_EXISTS);
    }

    ~FirstProcessGuard() { CloseHandle(appMutex); }

    HANDLE appMutex;
    bool isFirstInstance;

#else
    static constexpr auto PIDFile = "simplege.pid";

    static std::filesystem::path GetPIDFilePath()
    {
      auto filePath = std::filesystem::temp_directory_path();
      filePath.append(PIDFile);
      return filePath;
    }

    static void OnProcessExit() { std::filesystem::remove(GetPIDFilePath()); }

    static bool IsFirstProcessInstanceImpl()
    {
      std::filesystem::path pidFilePath = GetPIDFilePath();
      ::pid_t selfPID = ::getpid();
      if (std::filesystem::exists(pidFilePath))
      {
        std::ifstream pidFile(pidFilePath);
        ::pid_t pid{};
        pidFile >> pid;
        if (pid == selfPID)
        {
          return true;
        }

        if (::kill(pid, 0) != -1 || errno != ESRCH)
        {
          return false;
        }
      }
      std::ofstream newPIDFile(pidFilePath);
      newPIDFile << selfPID;

      std::atexit(&OnProcessExit);
      return true;
    }
#endif
  };
} // namespace SimpleGE