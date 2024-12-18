#pragma once

#include "chickendodge/pch/precomp.h"

#include <mutex>
#include <thread>

 #define USE_DEBUG_LEADERBOARD

namespace ChickenDodge
{
  class NetworkLeaderboardComponent : public SimpleGE::NetworkComponent, public SimpleGE::LogicComponent
  {
  public:
    static constexpr auto LeaderboardSize = 10;

    static constexpr auto Type = "NetworkLeaderboard";

    NetworkLeaderboardComponent(SimpleGE::Entity& owner) : Component(owner) {}

    void OnMessage(SimpleGE::Network::Connection& connection, const SimpleGE::BaseMessage& msg) override;

    void UpdateLogic(const SimpleGE::Timing& timing) override;

  private:
    void SetScore(const std::string& name, int score);

    std::unordered_map<std::string, int> scores;
    std::mutex mutex;

#ifdef USE_DEBUG_LEADERBOARD
    // Cette structure met à jour un score fictif afin de valider le
    // fonctionnement du système. À effacer lorsque l'implémentation
    // est complète.
    struct DebugLeaderboardTest
    {
      DebugLeaderboardTest(NetworkLeaderboardComponent& owner, std::string_view name, int score,
                           std::chrono::milliseconds freq);
      ~DebugLeaderboardTest();
      void Run();

      NetworkLeaderboardComponent& owner;
      std::string name;
      int score;
      std::chrono::milliseconds freq;

      std::thread thread;
      bool done{};
    };

    // On utilisera une frequence aléatoire pour bien vérifier que la synchronisation est correctement effectuée
    // Bizarrement sur le code que je télécharge depuis le moodle les rubis n'ajoutent pas de valeurs aux leaderboard...
    DebugLeaderboardTest debug1 = {*this, "Test 1", 1234, std::chrono::seconds(std::rand() % 10)};
    DebugLeaderboardTest debug2 = {*this, "Test 2", 750, std::chrono::seconds(std::rand() % 10)};
#endif // USE_DEBUG_LEADERBOARD
  };
} // namespace ChickenDodge