#include "chickendodge/pch/precomp.h"

#include "chickendodge/components/networkplayermanager.h"

#include "chickendodge/components/player.h"

#include "chickendodge/messages/networklogin.h"
#include "chickendodge/messages/networkstart.h"

using json = nlohmann::json;

using namespace SimpleGE;

namespace ChickenDodge
{
  static void from_json(const json& j, NetworkPlayerManagerComponent::Description::PlayerEntry& entry)
  {
    j.at("player").get_to(entry.player);
    j.at("input").get_to(entry.input);
  }

  static void from_json(const json& j, NetworkPlayerManagerComponent::Description& desc)
  {
    j.at("players").get_to(desc.players);
  }

  std::string NetworkPlayerManagerComponent::localPlayerName{};

  Component::SetupResult NetworkPlayerManagerComponent::Setup(const nlohmann::json& descr)
  {
    return Setup(descr.get<Description>());
  }

  Component::SetupResult NetworkPlayerManagerComponent::Setup(const Description& descr)
  {
    players = descr.players;
    NetworkLogin msg(localPlayerName);
    NetworkSystem::Send(msg);

    return {this, {}};
  }

  void NetworkPlayerManagerComponent::OnMessage(Network::Connection& connection, const BaseMessage& msg)
  {
    if (!msg.Is<NetworkStart>())
    {
      return;
    }

    const auto& startMsg = msg.Get<NetworkStart>();

    for (int i = 0; i < std::size(startMsg.names); i++)
    {
      bool isLocal = (i == startMsg.playerIndex);
      auto& entry = gsl::at(players, i);
      entry.player->SetName(startMsg.names[i]);
      entry.player->SetLocal(isLocal);
      entry.input->SetLocal(isLocal);
    }

    readyEvent.Trigger(startMsg.playerIndex);
  }
} // namespace ChickenDodge
