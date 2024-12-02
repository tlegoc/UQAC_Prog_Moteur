#pragma once

#include "chickendodge/pch/precomp.h"

namespace ChickenDodge
{
  class NetworkLogin;

  class NetworkPlayerServerComponent : public SimpleGE::NetworkComponent
  {
  private:
    struct SocketData
    {
      SocketData(SimpleGE::Network::ConnectionRef player) : player(std::move(player)) {}

      SimpleGE::Network::ConnectionRef player;
      std::weak_ptr<SimpleGE::Network::Connection> otherPlayer;
      std::string name;
    };

  public:
    static constexpr auto Type = "NetworkPlayerServer";

    static void OnConnect(const SimpleGE::Network::ConnectionRef& connection) { Instance().OnConnectImpl(connection); }

    static void OnDisconnect(gsl::not_null<SimpleGE::Network::Connection*> connection)
    {
      Instance().OnDisconnectImpl(connection);
    }

    NetworkPlayerServerComponent(SimpleGE::Entity& owner) : Component(owner)
    {
      Expects(InstancePtr() == nullptr);
      InstancePtr() = this;
    }

    ~NetworkPlayerServerComponent() override { InstancePtr() = nullptr; }

    void OnMessage(SimpleGE::Network::Connection& connection, const SimpleGE::BaseMessage& msg) override;

  private:
    [[nodiscard]] static NetworkPlayerServerComponent& Instance()
    {
      NetworkPlayerServerComponent* instance = InstancePtr();
      Expects(instance != nullptr);
      return *instance;
    }

    [[nodiscard]] static NetworkPlayerServerComponent*& InstancePtr()
    {
      static NetworkPlayerServerComponent* instance{};
      return instance;
    }

    void OnNetworkLogin(SimpleGE::Network::Connection& connection, const NetworkLogin& msg);
    void OnConnectImpl(const SimpleGE::Network::ConnectionRef& connection);
    void OnDisconnectImpl(gsl::not_null<SimpleGE::Network::Connection*> connection);

    std::unordered_map<SimpleGE::Network::Connection::ConnectionIDType, SocketData> clients;
    std::unordered_set<SimpleGE::Network::Connection::ConnectionIDType> pendingPlayers;
  };
} // namespace ChickenDodge