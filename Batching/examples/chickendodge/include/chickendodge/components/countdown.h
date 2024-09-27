#pragma once

#include "chickendodge/pch/precomp.h"

namespace ChickenDodge
{
  class NetworkPlayerManagerComponent;

  class CountdownComponent : public SimpleGE::LogicComponent
  {
  public:
    static constexpr auto Type = "Countdown";

    struct Description
    {
      std::vector<std::string> sprites;
      nlohmann::json spriteTemplate;
      std::chrono::milliseconds delay;
    };

    CountdownComponent(SimpleGE::Entity& owner) : Component(owner) {}

    SetupResult Setup(const nlohmann::json& descr) override;
    SetupResult Setup(const Description& descr);

    void UpdateLogic(const SimpleGE::Timing& timing) override;

  private:
    void SetupImpl(const Description& descr);
    void ShowImage();
    void ShowNamedImage(const std::string& textureName);

    std::vector<std::string> sprites;
    nlohmann::json spriteTemplate;
    std::chrono::milliseconds delay{};

    int index{-1};
    std::chrono::milliseconds shownTime{};
    SimpleGE::EntityRef current;
  };
} // namespace ChickenDodge