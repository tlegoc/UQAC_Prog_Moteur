#pragma once

#include "chickendodge/pch/precomp.h"

namespace ChickenDodge
{
  class GameInstructionsComponent : public SimpleGE::LogicComponent
  {
  public:
    static constexpr auto Type = "GameInstructions";

    GameInstructionsComponent(SimpleGE::Entity& owner) : Component(owner) {}

    void UpdateLogic(const SimpleGE::Timing& timing) override;
  };
} // namespace ChickenDodge