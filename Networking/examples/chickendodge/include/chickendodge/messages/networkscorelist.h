//
// Created by theo on 17/12/2024.
//

#pragma once

namespace ChickenDodge
{
  struct NetworkScoreList : public SimpleGE::Message<NetworkScoreList>
  {
    static constexpr MessageType Type = FirstGameType + 4;

    std::unordered_map<std::string, int> scores;

    NetworkScoreList() = default;
    NetworkScoreList(std::unordered_map<std::string, int> scores) : scores(scores) {}

    void Serialize(SimpleGE::ISerializer& serializer) const override
    {
      BaseMessage::Serialize(serializer);
      serializer.Write(scores.size());
      for(auto score : scores)
      {
        serializer.Write(score.first);
        serializer.Write(score.second);
      }
    }

    void Deserialize(SimpleGE::IDeserializer& deserializer) override
    {
      BaseMessage::Deserialize(deserializer);
      std::unordered_map<std::string, int>::size_type size = 0;
      deserializer.Read(size);
      for (int i = 0; i < size; i++)
      {
        std::string name;
        int score = 0;
        deserializer.Read(name);
        deserializer.Read(score);

        scores[name] = score;
      }
    }
  };
} // namespace ChickenDodge
