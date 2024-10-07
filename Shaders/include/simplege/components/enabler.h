#pragma once

#include <simplege/pch/precomp.h>

#include <simplege/components/component.h>

namespace SimpleGE
{
  class EnablerComponent : public Component
  {
  public:
    static constexpr auto Type = "Enabler";

    struct Description
    {
      std::vector<std::pair<ComponentReference<Component>, bool>> onStart;
      std::vector<std::pair<ComponentReference<Component>, bool>> onEvent;
    };

    EnablerComponent(Entity& owner) : Component(owner) {}

    SetupResult Setup(const nlohmann::json& descr) override;
    SetupResult Setup(const Description& descr);

    void OnEvent();

  private:
    decltype(Description::onEvent) onEvent;
  };
} // namespace SimpleGE
