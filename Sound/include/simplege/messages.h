#pragma once

#include <simplege/pch/precomp.h>

#include <simplege/messages/networkinputchanged.h>

namespace SimpleGE
{
  static inline void RegisterGenericMessages() { BaseMessage::Register<NetworkInputChanged>(); }
} // namespace SimpleGE