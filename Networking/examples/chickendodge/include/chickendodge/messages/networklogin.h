#pragma once

#include "chickendodge/pch/precomp.h"

namespace ChickenDodge
{
  struct NetworkLogin : public SimpleGE::Message<NetworkLogin>
  {
    static constexpr MessageType Type = FirstGameType + 1;

    std::string name;

    NetworkLogin() = default;
    NetworkLogin(std::string_view name) : name(name) {}

    void Serialize(SimpleGE::ISerializer& serializer) const override
    {
      BaseMessage::Serialize(serializer);
      serializer.Write(name);
    }

    void Deserialize(SimpleGE::IDeserializer& deserializer) override
    {
      BaseMessage::Deserialize(deserializer);
      deserializer.Read(name);
    }
  };
} // namespace ChickenDodge