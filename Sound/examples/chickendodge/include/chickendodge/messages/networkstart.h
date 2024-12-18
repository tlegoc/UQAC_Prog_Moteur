#pragma once

#include "chickendodge/pch/precomp.h"

namespace ChickenDodge
{
  struct NetworkStart : public SimpleGE::Message<NetworkStart>
  {
    static constexpr MessageType Type = FirstGameType + 2;

    int playerIndex;
    std::vector<std::string> names;

    NetworkStart() = default;
    NetworkStart(int playerIndex, std::initializer_list<std::string> names) : playerIndex(playerIndex), names(names) {}

    void Serialize(SimpleGE::ISerializer& serializer) const override
    {
      BaseMessage::Serialize(serializer);
      serializer.Write(playerIndex);

      serializer.Write(std::size(names));
      for (const auto& n : names)
      {
        serializer.Write(n);
      }
    }

    void Deserialize(SimpleGE::IDeserializer& deserializer) override
    {
      BaseMessage::Deserialize(deserializer);
      deserializer.Read(playerIndex);

      decltype(names)::size_type nbNames{};
      deserializer.Read(nbNames);
      names.resize(nbNames);
      for (int i = 0; i < nbNames; i++)
      {
        deserializer.Read(gsl::at(names, i));
      }
    }
  };
} // namespace ChickenDodge