#include "chickendodge/pch/precomp.h"

#include "networkplayerserver.h"

#include "chickendodge/messages/networklogin.h"
#include "chickendodge/messages/networkstart.h"

#include <chickendodge/messages/networkscore.h>
#include <chickendodge/messages/networkscorelist.h>

using namespace SimpleGE;

namespace ChickenDodge
{
  void NetworkPlayerServerComponent::OnMessage(Network::Connection& connection, const BaseMessage& msg)
  {
    fmt::print("Message de {}\n", connection.GetID());
    if (msg.Is<NetworkLogin>())
    {
      OnNetworkLogin(connection, msg.Get<NetworkLogin>());
    }
    if (msg.Is<NetworkInputChanged>())
    {
      auto otherPlayer = clients.find(connection.GetID())->second.otherPlayer.lock();
      if (otherPlayer != nullptr)
      {
        NetworkSystem::Send(otherPlayer.get(), msg);
      }
    }
    if (msg.Is<NetworkScore>())
    {
      auto scoreMsg = msg.Get<NetworkScore>();
      fmt::print("Message de score recu: {} (score {})\n", scoreMsg.name, scoreMsg.score);
      if (scoreMsg.score > scores[scoreMsg.name])
      {
        scores[scoreMsg.name] = scoreMsg.score;
        NetworkScoreList scoreUpdate(scores);

        // Update all other clients except the one that sent the message (avoid updating an already up to date client)
        for (const auto& client : clients)
        {
          if (client.first != connection.GetID())
          {
            NetworkSystem::Send(client.second.player.get(), scoreUpdate);
          }
        }
      }
    }
  }

  void NetworkPlayerServerComponent::OnNetworkLogin(Network::Connection& connection, const NetworkLogin& msg)
  {
    auto id = connection.GetID();
    auto& socketData = clients.find(id)->second;
    socketData.name = msg.name;


    // Send the score
    // Do this here because we need to do it even if
    NetworkScoreList scoreUpdate(scores);
    NetworkSystem::Send(&connection, scoreUpdate);

    // Si aucun joueur n'est en attente, on place le nouveau
    // joueur en attente.
    if (pendingPlayers.empty())
    {
      pendingPlayers.insert(id);
      return;
    }

    // Si il y a des joueurs en attente, on associe un de
    // ces joueurs à celui-ci.
    auto otherId = *pendingPlayers.begin();
    pendingPlayers.erase(otherId);

    auto& otherSocketData = clients.find(otherId)->second;
    socketData.otherPlayer = otherSocketData.player;
    otherSocketData.otherPlayer = socketData.player;

    // On envoie alors la liste des joueurs de la partie
    // à chacun des participants.
    NetworkStart p1(0, {otherSocketData.name, socketData.name});
    NetworkStart p2(1, {otherSocketData.name, socketData.name});

    NetworkSystem::Send(otherSocketData.player.get(), p1);
    NetworkSystem::Send(socketData.player.get(), p2);
  }

  void NetworkPlayerServerComponent::OnConnectImpl(const Network::ConnectionRef& connection)
  {
    fmt::print("Connexion de {}\n", connection->GetID());
    Expects(clients.find(connection->GetID()) == clients.end());
    clients.emplace(connection->GetID(), connection);
  }

  void NetworkPlayerServerComponent::OnDisconnectImpl(gsl::not_null<Network::Connection*> connection)
  {
    auto id = connection->GetID();
    fmt::print("Déconnexion de {}\n", id);
    Expects(clients.find(id) != clients.end());

    auto& socketData = clients.find(id)->second;
    auto otherPlayer = socketData.otherPlayer.lock();
    if (otherPlayer != nullptr)
    {
      clients.erase(otherPlayer->GetID());
    }

    pendingPlayers.erase(id);
    clients.erase(id);
  }
} // namespace ChickenDodge