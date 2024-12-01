#pragma once

#include "chickendodge/pch/precomp.h"

namespace ChickenDodge
{
  class PlayerComponent;

  class RefereeComponent : public SimpleGE::LogicComponent
  {
  public:
    static constexpr auto Type = "Referee";

    using WinEventType = std::function<void(PlayerComponent&, PlayerComponent&)>;

    struct Description
    {
      std::array<SimpleGE::ComponentReference<PlayerComponent>, 2> players;
    };

    RefereeComponent(SimpleGE::Entity& owner) : Component(owner) {}

    SetupResult Setup(const nlohmann::json& descr) override;
    SetupResult Setup(const Description& descr);

    void UpdateLogic(const SimpleGE::Timing& timing) override;

    void RegisterWinEvent(const WinEventType& onWin) { winEvent.Register(onWin); }

  private:
    void SetupImpl(const Description& descr);
    void OnDead();

    SimpleGE::EventTrigger<RefereeComponent::WinEventType> winEvent;
    std::array<SimpleGE::ComponentReference<PlayerComponent>, 2> players;

    struct WinInfo
    {
      std::string winner;
      std::string loser;
    };
    std::optional<WinInfo> winInfo;
  };
} // namespace ChickenDodge