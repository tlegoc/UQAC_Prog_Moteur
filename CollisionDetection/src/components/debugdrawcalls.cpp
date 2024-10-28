#include <simplege/pch/precomp.h>

#include <simplege/simplege.h>

#include <imgui.h>

namespace SimpleGE
{
  void DebugDrawCallsComponent::UpdateLogic(const Timing& timing)
  {
    auto calls = GFXAPI::CommandBuffer::GetAndResetDrawCount();

    if (GUISystem::IsInitialized())
    {
      auto viewSize = ImGui::GetMainViewport()->WorkSize;

      ImGui::Begin("Statistiques");
      ImGui::Text("Nb appels de rendu: %d", calls);
      ImGui::SetWindowSize({viewSize.x * 0.25f, ImGui::GetWindowHeight()}, ImGuiCond_Once);
      ImGui::SetWindowPos({0, viewSize.y - ImGui::GetWindowHeight()}, ImGuiCond_Once);
      ImGui::End();
    }
  }
} // namespace SimpleGE
