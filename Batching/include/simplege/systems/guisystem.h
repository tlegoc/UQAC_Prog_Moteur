#pragma once

#include <simplege/pch/precomp.h>

#include <simplege/systems/system.h>

namespace SimpleGE
{
  class GUISystem : public ISystem
  {
  public:
    static GUISystem& Instance()
    {
      static GUISystem instance;
      return instance;
    }

    GUISystem(const GUISystem&) = delete;
    GUISystem(GUISystem&&) = delete;
    GUISystem& operator=(const GUISystem&) = delete;
    GUISystem& operator=(GUISystem&&) = delete;
    ~GUISystem() override;

    [[nodiscard]] static bool IsInitialized();

  private:
    GUISystem();
  };
} // namespace SimpleGE