#pragma once

#include <simplege/pch/precomp.h>

#include <simplege/components/logic.h>
#include <simplege/entity.h>
#include <simplege/systems/system.h>

namespace SimpleGE
{
  class LogicSystem : public ISystem
  {
  public:
    [[nodiscard]] static LogicSystem& Instance()
    {
      static LogicSystem instance;
      return instance;
    }

    LogicSystem(const LogicSystem&) = delete;
    LogicSystem(LogicSystem&&) = delete;
    LogicSystem& operator=(const LogicSystem&) = delete;
    LogicSystem& operator=(LogicSystem&&) = delete;

    ~LogicSystem() override { Expects(logicComponents.empty()); }

    void Register(gsl::not_null<LogicComponent*> comp) { logicComponents.insert(comp); }

    void Unregister(gsl::not_null<LogicComponent*> comp) { logicComponents.erase(comp); }

    void Iterate(const Timing& timing) override
    {
      for (auto logComp : logicComponents)
      {
        if (logComp->Enabled() && logComp->Owner().Active())
        {
          logComp->UpdateLogic(timing);
        }
      }
    }

  private:
    LogicSystem() = default;

    std::unordered_set<gsl::not_null<LogicComponent*>> logicComponents;
  };

  inline LogicComponent::LogicComponent() { LogicSystem::Instance().Register(this); }

  inline LogicComponent::~LogicComponent() { LogicSystem::Instance().Unregister(this); }
} // namespace SimpleGE