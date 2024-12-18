#include <simplege/pch/precomp.h>

#include <simplege/simplege.h>

#include <simplege/messages/networkinputchanged.h>

using json = nlohmann::json;

namespace SimpleGE
{
  static void from_json(const json& j, NetworkInputComponent::Description& desc)
  {
    j.at("localInput").get_to(desc.localInput);
  }

  Component::SetupResult NetworkInputComponent::Setup(const nlohmann::json& descr)
  {
    return Setup(descr.get<Description>());
  }

  Component::SetupResult NetworkInputComponent::Setup(const Description& descr)
  {
    localInput = descr.localInput;

    return {this, {}};
  }

  void NetworkInputComponent::UpdateLogic(const Timing& timing)
  {
    if (IsLocal())
    {
      UpdateLocal();
    }
  }

  void NetworkInputComponent::OnMessage(Network::Connection& connection, const BaseMessage& msg)
  {
    if (IsLocal())
    {
      return;
    }

    if (msg.Is<NetworkInputChanged>())
    {
      UpdateRemote(msg.Get<NetworkInputChanged>());
    }
  }

  void NetworkInputComponent::UpdateLocal()
  {
    bool changed = false;
    for (int k = 0; k < std::size(inputStatus); k++)
    {
      bool newVal = localInput->IsActive((NetworkInputComponent::Action) k);
      if (newVal != gsl::at(inputStatus, k))
      {
        changed = true;
        gsl::at(inputStatus, k) = newVal;
      }
    }

    if (changed)
    {
      NetworkInputChanged msg(inputStatus);
      NetworkSystem::Send(msg);
    }
  }

  void NetworkInputComponent::UpdateRemote(const NetworkInputChanged& msg)
  {
    std::copy(msg.symbols.begin(), msg.symbols.end(), inputStatus.begin());
  }
} // namespace SimpleGE
