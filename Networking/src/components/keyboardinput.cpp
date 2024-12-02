#include <simplege/pch/precomp.h>

#include <simplege/simplege.h>

using json = nlohmann::json;

namespace SimpleGE
{
  static const std::unordered_map<std::string, InputComponent::Action> ActionMappings = {
      {"up", InputComponent::Action::Up},         {"down", InputComponent::Action::Down},
      {"left", InputComponent::Action::Left},     {"right", InputComponent::Action::Right},
      {"attack", InputComponent::Action::Attack},
  };

  static void from_json(const json& j, KeyboardInputComponent::Description& desc)
  {
    for (const auto& action : ActionMappings)
    {
      desc.mapping[action.second] = j.at("mapping").at(action.first).get<std::string>();
    }
  }

  Component::SetupResult KeyboardInputComponent::Setup(const nlohmann::json& descr)
  {
    return Setup(descr.get<Description>());
  }

  Component::SetupResult KeyboardInputComponent::Setup(const Description& descr)
  {
    mapping = descr.mapping;
    return {this, {}};
  }

  [[nodiscard]] bool KeyboardInputComponent::IsActive(Action action) const
  {
    return DisplaySystem::Instance().GetWindow().IsKeyDown(Graphic::GetKeyCode(mapping.at(action)));
  }
} // namespace SimpleGE
