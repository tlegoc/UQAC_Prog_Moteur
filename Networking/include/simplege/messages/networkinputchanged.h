#pragma once

#include <simplege/pch/precomp.h>

#include <simplege/messages/message.h>

#include <simplege/components/networkinput.h>

namespace SimpleGE
{
  struct NetworkInputChanged : public Message<NetworkInputChanged>
  {
    static constexpr MessageType Type = FirstEngineType + 1;

    using SymbolsContainer = std::array<bool, (int) NetworkInputComponent::Action::Count>;

    SymbolsContainer symbols;

    NetworkInputChanged() = default;
    NetworkInputChanged(const SymbolsContainer& symbols) : symbols(symbols) {}

    void Serialize(ISerializer& serializer) const override
    {
      BaseMessage::Serialize(serializer);
      for (bool s : symbols)
      {
        serializer.Write(s);
      }
    }

    void Deserialize(IDeserializer& deserializer) override
    {
      BaseMessage::Deserialize(deserializer);
      for (bool& s : symbols)
      {
        deserializer.Read(s);
      }
    }
  };
} // namespace SimpleGE