#include "chickendodge/pch/precomp.h"

#include "chickendodge/components/gameinstructions.h"
#include "chickendodge/components/player.h"

#include <imgui.h>

using json = nlohmann::json;

using namespace SimpleGE;

namespace ChickenDodge
{
  void GameInstructionsComponent::UpdateLogic(const Timing& timing)
  {
    if (GUISystem::IsInitialized())
    {
      const auto& names = PlayerComponent::GetNames();

      ImGui::Begin("Instructions");
      ImGui::Text("%s (%s):", gsl::at(names, 0).c_str(), SimpleGE::Localisation::Get("greenPlayer").c_str());
      ImGui::Text("  %s: %s", SimpleGE::Localisation::Get("CHAR_KEYS").c_str(),
                  SimpleGE::Localisation::Get("MOVE").c_str());
      ImGui::Text("  %s: %s", SimpleGE::Localisation::Get("SPACE_KEY").c_str(),
                  SimpleGE::Localisation::Get("ATTACK").c_str());

      ImGui::Text("%s (%s):", gsl::at(names, 1).c_str(), SimpleGE::Localisation::Get("redPlayer").c_str());
      ImGui::Text("  %s: %s", SimpleGE::Localisation::Get("ARROW_KEYS").c_str(),
                  SimpleGE::Localisation::Get("MOVE").c_str());
      ImGui::Text("  %s: %s", SimpleGE::Localisation::Get("ENTER_KEY").c_str(),
                  SimpleGE::Localisation::Get("ATTACK").c_str());
      ImGui::End();
    }
  }
} // namespace ChickenDodge
