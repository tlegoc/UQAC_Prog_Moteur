#pragma once

#include <simplege/pch/precomp.h>

#include <simplege/components/component.h>
#include <simplege/messages/message.h>

namespace SimpleGE
{
  namespace Network
  {
    class Connection;
  }

  class NetworkComponent : public virtual Component
  {
  public:
    inline NetworkComponent();
    inline ~NetworkComponent() override;

    virtual void OnMessage(Network::Connection& connection, const BaseMessage& msg) = 0;
  };
} // namespace SimpleGE
