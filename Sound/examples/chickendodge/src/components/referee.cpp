#include "chickendodge/pch/precomp.h"

#include "chickendodge/components/referee.h"

#include "chickendodge/components/player.h"
#include "chickendodge/components/score.h"

#include <imgui.h>

#include <iostream>

using json = nlohmann::json;

using namespace SimpleGE;

namespace ChickenDodge
{
  static void from_json(const json& j, RefereeComponent::Description& desc) { j.at("players").get_to(desc.players); }

  Component::SetupResult RefereeComponent::Setup(const nlohmann::json& descr)
  {
    return Setup(descr.get<Description>());
  }

  Component::SetupResult RefereeComponent::Setup(const Description& descr)
  {
    winEvent.Register(
        [this](auto& winner, auto& loser)
        {
          winInfo = {
              .winner = std::string(winner.GetName()),
              .loser = std::string(loser.GetName()),
          };
        });

    return {this,
            {{[descr]() {
                return std::all_of(descr.players.begin(), descr.players.end(),
                                   [](const auto& player) { return player.Ready(); });
              },
              [this, descr]() { SetupImpl(descr); }}}};
  }

  void RefereeComponent::SetupImpl(const Description& descr)
  {
    std::transform(descr.players.begin(), descr.players.end(), players.begin(),
                   [this](const auto& player)
                   {
                     player->RegisterDeadEvent([this]() { OnDead(); });
                     return player;
                   });
  }

  void RefereeComponent::OnDead()
  {
    int bestScore{-1};
    PlayerComponent* bestPlayer{};
    int worstScore{std::numeric_limits<int>::max()};
    PlayerComponent* worstPlayer{};

    bool gameOver{true};

    for (const auto& p : players)
    {
      if (!gameOver)
      {
        continue;
      }
      if (!p->IsDead())
      {
        gameOver = false;
        continue;
      }

      int score = p->GetScore()->GetValue();
      if (score > bestScore)
      {
        bestScore = score;
        bestPlayer = p.Resolve();
      }
      if (score < worstScore)
      {
        worstScore = score;
        worstPlayer = p.Resolve();
      }
    }

    if (gameOver)
    {
      Ensures(bestPlayer != nullptr);
      Ensures(worstPlayer != nullptr);
      winEvent.Trigger(*bestPlayer, *worstPlayer);

      AudioSystem::SendEvent("end");
    }
  }

  void RefereeComponent::UpdateLogic(const SimpleGE::Timing& timing)
  {
    if (winInfo.has_value())
    {
      LocaleContext queryContext{
          {"WINNER", winInfo->winner},
          {"LOSER", winInfo->loser},
      };
      ImGui::OpenPopup("WinMessage");
      if (ImGui::BeginPopupModal("WinMessage", nullptr, ImGuiWindowFlags_NoTitleBar))
      {
        ImGui::Text("%s", Localisation::Get("winMessage", queryContext).c_str());
        ImGui::EndPopup();
      }
    }
  }

} // namespace ChickenDodge
