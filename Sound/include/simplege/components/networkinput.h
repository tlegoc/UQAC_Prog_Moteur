#pragma once

#include <simplege/pch/precomp.h>

#include <simplege/components/input.h>
#include <simplege/components/logic.h>
#include <simplege/components/network.h>

namespace SimpleGE
{
  struct NetworkInputChanged;

  class NetworkInputComponent : public NetworkComponent, public InputComponent, public LogicComponent
  {
  public:
    static constexpr auto Type = "NetworkInput";

    struct Description
    {
      ComponentReference<InputComponent> localInput;
    };

    NetworkInputComponent(Entity& owner) : Component(owner) {}

    SetupResult Setup(const nlohmann::json& descr) override;
    SetupResult Setup(const Description& descr);

    void UpdateLogic(const Timing& timing) override;
    void OnMessage(Network::Connection& connection, const BaseMessage& msg) override;
    [[nodiscard]] bool IsActive(Action action) const override { return gsl::at(inputStatus, (int) action); }

    void SetLocal(bool isLocal) { this->isLocal = isLocal; }
    [[nodiscard]] bool IsLocal() const { return isLocal; }

  private:
    void UpdateLocal();
    void UpdateRemote(const NetworkInputChanged& msg);

    ComponentReference<InputComponent> localInput;
    bool isLocal{true};
    std::array<bool, (int) NetworkInputComponent::Action::Count> inputStatus{};
  };
} // namespace SimpleGE
