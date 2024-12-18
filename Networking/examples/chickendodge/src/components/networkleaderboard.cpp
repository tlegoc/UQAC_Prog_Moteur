#include "chickendodge/pch/precomp.h"

#include "chickendodge/components/networkleaderboard.h"
#include "chickendodge/messages/networkscore.h"
#include "fmt/printf.h"

#include <imgui.h>
#include <chickendodge/messages/networkscorelist.h>

using json = nlohmann::json;

using namespace SimpleGE;

namespace ChickenDodge
{
  void NetworkLeaderboardComponent::OnMessage(Network::Connection& connection, const BaseMessage& msg)
  {
    if (msg.Is<NetworkScoreList>())
    {
      const auto& scoreMsg = msg.Get<NetworkScoreList>();

      fmt::print("Score recu\n");

      std::lock_guard<std::mutex> lock(mutex);
      scores = scoreMsg.scores;
    }
  }

  void NetworkLeaderboardComponent::SetScore(const std::string& name, int score)
  {
    std::lock_guard<std::mutex> lock(mutex);
    scores[name] = score;

    NetworkScore msg(name, score);
    NetworkSystem::Send(msg);
  }

  void NetworkLeaderboardComponent::UpdateLogic(const SimpleGE::Timing& timing)
  {
    std::lock_guard<std::mutex> lock(mutex);
    std::vector<std::pair<std::string, int>> sorted(scores.begin(), scores.end());
    std::sort(sorted.begin(), sorted.end(), [](const auto& a, const auto& b) { return a.second > b.second; });

    if (GUISystem::IsInitialized())
    {
      auto viewSize = ImGui::GetMainViewport()->WorkSize;

      ImGui::SetNextWindowSize({viewSize.x * 0.2f, viewSize.y * 0.25f});
      ImGui::SetNextWindowPos({viewSize.x * 0.8f, viewSize.y * 0.75f}, ImGuiCond_Once);
      ImGui::Begin("Leaderboard");
      ImGui::BeginTable("Leaderboard", 2);
      for (int i = 0; i < LeaderboardSize && i < std::size(sorted); i++)
      {
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::Text("%s", sorted[i].first.c_str());
        ImGui::TableNextColumn();
        ImGui::Text("%d", sorted[i].second);
      }
      ImGui::EndTable();
      ImGui::End();
    }
  }

#ifdef USE_DEBUG_LEADERBOARD
  NetworkLeaderboardComponent::DebugLeaderboardTest::DebugLeaderboardTest(NetworkLeaderboardComponent& owner,
                                                                          std::string_view name, int score,
                                                                          std::chrono::milliseconds freq)
    : owner(owner), name(name), score(score), freq(freq), thread([this]() { Run(); })
  {
  }

  NetworkLeaderboardComponent::DebugLeaderboardTest::~DebugLeaderboardTest()
  {
    done = true;
    thread.join();
  }

  void NetworkLeaderboardComponent::DebugLeaderboardTest::Run()
  {
    done = false;
    while (!done)
    {
      owner.SetScore(name, score);
      score += 250;
      std::this_thread::sleep_for(freq);
    }
  }
#endif // USE_DEBUG_LEADERBOARD
} // namespace ChickenDodge