#pragma once

#include "chickendodge/pch/precomp.h"

#include "chickendodge/messages/networklogin.h"
#include "chickendodge/messages/networkstart.h"
#include "chickendodge/messages/networkscore.h"
#include "chickendodge/messages/networkscorelist.h"

namespace ChickenDodge
{
  static inline void RegisterGameMessages()
  {
    using Message = SimpleGE::BaseMessage;

    Message::Register<NetworkLogin>();
    Message::Register<NetworkStart>();
    Message::Register<NetworkScore>();
    Message::Register<NetworkScoreList>();
  }
} // namespace ChickenDodge