#include <simplege/pch/precomp.h>

#include <simplege/simplege.h>

using json = nlohmann::json;

namespace SimpleGE
{
  static void from_json(const json& j, EnablerComponent::Description& desc)
  {
    desc.onStart.clear();
    for (const auto& entry : j.at("onStart").get<std::unordered_map<std::string, bool>>())
    {
      desc.onStart.emplace_back(entry);
    }
    desc.onEvent.clear();
    for (const auto& entry : j.at("onEvent").get<std::unordered_map<std::string, bool>>())
    {
      desc.onEvent.emplace_back(entry);
    }
  }

  Component::SetupResult EnablerComponent::Setup(const nlohmann::json& descr)
  {
    return Setup(descr.get<Description>());
  }

  Component::SetupResult EnablerComponent::Setup(const Description& descr)
  {
    onEvent = descr.onEvent;

    return {this,
            {{[descr]()
              {
                return std::all_of(descr.onStart.begin(), descr.onStart.end(),
                                   [](const auto& entry) { return entry.first.Ready(); });
              },
              [descr]()
              {
                for (const auto& entry : descr.onStart)
                {
                  entry.first->SetEnabled(entry.second);
                }
              }}}};
  }

  void EnablerComponent::OnEvent()
  {
    for (const auto& entry : onEvent)
    {
      entry.first->SetEnabled(entry.second);
    }
  }
} // namespace SimpleGE