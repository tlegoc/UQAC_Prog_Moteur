#include "chickendodge/pch/precomp.h"

#include "chickendodge/components/gameinstructions.h"

#include <imgui.h>

using json = nlohmann::json;

using namespace SimpleGE;

namespace ChickenDodge
{
  void GameInstructionsComponent::UpdateLogic(const Timing& timing)
  {
    if (GUISystem::IsInitialized())
    {
      ImGui::Begin("Instructions");
      ImGui::Text("%s: %s", SimpleGE::Localisation::Get("CHAR_KEYS").c_str(),
                  SimpleGE::Localisation::Get("MOVE").c_str());
      ImGui::Text("%s: %s", SimpleGE::Localisation::Get("SPACE_KEY").c_str(),
                  SimpleGE::Localisation::Get("ATTACK").c_str());
      ImGui::End();
    }
  }
} // namespace ChickenDodge
