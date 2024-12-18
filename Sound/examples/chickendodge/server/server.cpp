#include "chickendodge/pch/precomp.h"

#include "chickendodge/components.h"
#include "chickendodge/messages.h"
#include "networkplayerserver.h"

constexpr auto LaunchScene = "scenes/server.json";
constexpr std::uint16_t DefaultPort = 4321;

namespace ChickenDodge
{
  struct Config
  {
    static Config& Instance()
    {
      static Config instance;
      return instance;
    }

    static void SetPort(std::string_view val) { Instance().port = std::atoi(val.data()); }

    std::uint16_t port{DefaultPort};
  };

  class Server : public SimpleGE::Game
  {
  public:
    Server()
    {
      server = SimpleGE::Network::Listen(
          Config::Instance().port,
          [](const SimpleGE::Network::ConnectionRef& connection)
          { NetworkPlayerServerComponent::OnConnect(connection); },
          [](gsl::not_null<SimpleGE::Network::Connection*> connection)
          { NetworkPlayerServerComponent::OnDisconnect(connection); },
          [this](gsl::not_null<SimpleGE::Network::Connection*> connection, std::span<std::byte> data)
          { OnData(connection, data); });
    }

  protected:
    [[nodiscard]] std::string_view GetLaunchScene() const override { return LaunchScene; }

    void SetupSystem() override
    {
      AddSystem<SimpleGE::NetworkSystem>();
      AddSystem<SimpleGE::LogicSystem>();
    }

    void RegisterComponents() const override
    {
      RegisterGameComponents();
      RegisterServerComponents();
    }

    void RegisterMessages() const override { RegisterGameMessages(); }
    void RegisterShaderPrograms() const override {}

  private:
    static void RegisterServerComponents()
    {
      using Component = SimpleGE::Component;

      Component::Register<NetworkPlayerServerComponent>();
    }

    void OnData(gsl::not_null<SimpleGE::Network::Connection*> connection, std::span<std::byte> data)
    {
      SimpleGE::NetworkSystem::Recv(*connection, data);
    }

    SimpleGE::Network::ServerRef server;
  };

  static void ProcessCommandLine(std::span<char*> args)
  {
    SimpleGE::CommandLine::RegisterOption("-p", Config::SetPort);
    SimpleGE::CommandLine::Parse(args);
  }
} // namespace ChickenDodge

int main(int argc, char* argv[])
{
  ChickenDodge::ProcessCommandLine({argv, (size_t) argc});

  ChickenDodge::Server server;
  auto runResult = server.Run();
  return runResult == SimpleGE::Game::RunResult::Success ? 0 : 1;
}