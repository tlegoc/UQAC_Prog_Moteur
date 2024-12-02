#pragma once

#include <simplege/pch/precomp.h>

#include <simplege/components/network.h>
#include <simplege/entity.h>
#include <simplege/systems/system.h>

namespace SimpleGE
{
  class NetworkSystem : public ISystem
  {
  public:
    static constexpr auto SendBufferSize = 4096;

    [[nodiscard]] static NetworkSystem& Instance()
    {
      static NetworkSystem instance;
      return instance;
    }

    static void Send(gsl::not_null<Network::Connection*> connection, const BaseMessage& msg)
    {
      std::vector<std::byte> buffer;
      buffer.reserve(SendBufferSize);
      BinarySerializer serializer(buffer);
      msg.Serialize(serializer);

      connection->Send(buffer);
    }

    static void Send(const BaseMessage& msg)
    {
      const auto& connections = Network::ListConnections();
      Expects(std::size(connections) == 1);
      Send(*connections.begin(), msg);
    }

    static void Recv(Network::Connection& connection, std::span<std::byte> data)
    {
      BinaryDeserializer deserializer(data);
      BaseMessage::MessageType type{};
      deserializer.Peek(type);
      auto msg = BaseMessage::Create(type);
      msg->Deserialize(deserializer);

      for (auto comp : Instance().networkComponents)
      {
        comp->OnMessage(connection, *msg);
      }
    }

    NetworkSystem(const NetworkSystem&) = delete;
    NetworkSystem(NetworkSystem&&) = delete;
    NetworkSystem& operator=(const NetworkSystem&) = delete;
    NetworkSystem& operator=(NetworkSystem&&) = delete;

    ~NetworkSystem() override { Expects(networkComponents.empty()); }

    void Register(gsl::not_null<NetworkComponent*> comp) { networkComponents.insert(comp); }

    void Unregister(gsl::not_null<NetworkComponent*> comp) { networkComponents.erase(comp); }

    void Iterate(const Timing& timing) override { Network::Service(); }

  private:
    NetworkSystem() = default;

    std::unordered_set<gsl::not_null<NetworkComponent*>> networkComponents;
  };

  inline NetworkComponent::NetworkComponent() { NetworkSystem::Instance().Register(this); }

  inline NetworkComponent::~NetworkComponent() { NetworkSystem::Instance().Unregister(this); }
} // namespace SimpleGE