#pragma once

#include "chickendodge/pch/precomp.h"

namespace ChickenDodge
{
  class PlayerComponent;

  class NetworkPlayerManagerComponent : public SimpleGE::NetworkComponent
  {
  public:
    static constexpr auto Type = "NetworkPlayerManager";

    using PlayerReadyEventType = std::function<void(int)>;

    struct Description
    {
      struct PlayerEntry
      {
        SimpleGE::ComponentReference<PlayerComponent> player;
        SimpleGE::ComponentReference<SimpleGE::NetworkInputComponent> input;
      };

      std::vector<PlayerEntry> players;
    };

    static void SetLocalPlayerName(std::string_view name) { localPlayerName = name; }

    NetworkPlayerManagerComponent(SimpleGE::Entity& owner) : Component(owner) {}

    SetupResult Setup(const nlohmann::json& descr) override;
    SetupResult Setup(const Description& descr);

    void OnMessage(SimpleGE::Network::Connection& connection, const SimpleGE::BaseMessage& msg) override;

    void RegisterPlayerReadyEvent(const PlayerReadyEventType& onPlayerReady) { readyEvent.Register(onPlayerReady); }

  private:
    using PlayerEntry = NetworkPlayerManagerComponent::Description::PlayerEntry;

    static std::string localPlayerName;

    SimpleGE::EventTrigger<NetworkPlayerManagerComponent::PlayerReadyEventType> readyEvent;
    std::vector<PlayerEntry> players;
  };
} // namespace ChickenDodge