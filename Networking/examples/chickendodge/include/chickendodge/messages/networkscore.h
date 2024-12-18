//
// Created by theo on 09/12/2024.
//

#pragma once

#include "chickendodge/pch/precomp.h"

namespace ChickenDodge
{
  struct NetworkScore : public SimpleGE::Message<NetworkScore>
  {
    static constexpr MessageType Type = FirstGameType + 3;

    std::string name;
    int score;

    NetworkScore() = default;
    NetworkScore(std::string_view name, int score) : name(name), score(score) {}

    void Serialize(SimpleGE::ISerializer& serializer) const override
    {
      BaseMessage::Serialize(serializer);
      serializer.Write(name);
      serializer.Write(score);
    }

    void Deserialize(SimpleGE::IDeserializer& deserializer) override
    {
      BaseMessage::Deserialize(deserializer);
      deserializer.Read(name);
      deserializer.Read(score);
    }
  };
} // namespace ChickenDodge