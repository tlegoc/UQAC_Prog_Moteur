#pragma once

#include "chickendodge/pch/precomp.h"

#include "chickendodge/messages/networklogin.h"
#include "chickendodge/messages/networkstart.h"

namespace ChickenDodge
{
  static inline void RegisterGameMessages()
  {
    using Message = SimpleGE::BaseMessage;

    Message::Register<NetworkLogin>();
    Message::Register<NetworkStart>();
  }
} // namespace ChickenDodge