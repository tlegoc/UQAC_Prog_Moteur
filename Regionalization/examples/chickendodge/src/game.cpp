#include "chickendodge/pch/precomp.h"

#include "chickendodge/components.h"
#include "chickendodge/shaderprograms.h"

#include <yaml-cpp/yaml.h>

#ifdef NDEBUG
#undef NDEBUG
#endif
#include <cassert>

static const SimpleGE::LocaleFiles Locales = {
    {"en", "locales/en.json"},
    {"fr", "locales/fr.json"},
};

constexpr auto GameTitle = "Chicken Dodge";
constexpr auto DefaultWidth = 960;
constexpr auto DefaultHeight = 720;
constexpr auto TeamFile = "equipe.yaml";
constexpr auto LaunchScene = "scenes/play.json";
constexpr auto DefaultLang = "fr";
constexpr auto DefaultName = "Anonyme";

namespace ChickenDodge
{
  struct Config
  {
    static Config& Instance()
    {
      static Config instance;
      return instance;
    }

    static void SetName(std::string_view val) { Instance().alias = val; }

    static void SetOtherName(std::string_view val) { Instance().alias2 = val; }

    std::string alias{DefaultName};
    std::string alias2{DefaultName};
  };

  class Game : public SimpleGE::Game
  {
  public:
    Game() { PlayerComponent::SetNames({Config::Instance().alias, Config::Instance().alias2}); }

    ~Game() override = default;

    Game(const Game&) = delete;
    Game(Game&&) = delete;
    Game& operator=(const Game&) = delete;
    Game& operator=(Game&&) = delete;

  protected:
    [[nodiscard]] std::string_view GetLaunchScene() const override { return LaunchScene; }

    void SetupSystem() override
    {
      SimpleGE::DisplaySystem::SetConfig({
          .title = GameTitle,
          .width = DefaultWidth,
          .height = DefaultHeight,
      });

      AddSystem<SimpleGE::PhysicSystem>();
      AddSystem<SimpleGE::LogicSystem>();
      AddSystem<SimpleGE::DisplaySystem>();
      AddSystem<SimpleGE::GUISystem>();
    }

    void RegisterComponents() const override { RegisterGameComponents(); }
    void RegisterShaderPrograms() const override { RegisterGameShaderPrograms(); }
  };

  static void ProcessCommandLine(std::span<char*> args)
  {
    SimpleGE::CommandLine::RegisterOption("-U", Config::SetName);
    SimpleGE::CommandLine::RegisterOption("-O", Config::SetOtherName);
    SimpleGE::CommandLine::Parse(args);
  }
} // namespace ChickenDodge

[[nodiscard]] static bool ValidateTeam()
{
  auto teamFile = SimpleGE::Resources::Get<SimpleGE::TextAsset>(TeamFile);
  if (teamFile == nullptr)
  {
    fmt::print(stderr, "Échec d'ouverture de {}\n", TeamFile);
    return false;
  }

  YAML::Node root = YAML::Load(teamFile->Value().data());
  auto equipeNode = root["equipe"];
  if (equipeNode.Type() != YAML::NodeType::Map || std::size(equipeNode) == 0)
  {
    fmt::print(stderr, "N'oubliez pas d'inscrire les membres de votre équipe dans le fichier data/equipe.yaml!\n");
    return false;
  }

  std::regex codeMatch("^[A-Z]{4}[0-9]{8}$");

  for (const auto item : equipeNode)
  {
    auto code = item.first.as<std::string>();
    auto name = item.second.as<std::string>();
    if (!std::regex_match(code, codeMatch))
    {
      fmt::print(stderr, "Code permanent invalide: {}!\n", code);
      return false;
    }
  }

  return true;
}

static void SetupLocales()
{
  const char* lang = std::getenv("LANG");
  if (lang == nullptr)
  {
    lang = DefaultLang;
  }
  SimpleGE::Localisation::Init(Locales, std::string_view(lang, 2));
}

int main(int argc, char* argv[])
{
  if (!ValidateTeam())
  {
    assert(false);
    return -1;
  }

  SetupLocales();

  ChickenDodge::ProcessCommandLine({argv, (size_t) argc});

  ChickenDodge::Game game;
  auto runResult = game.Run();
  return runResult == SimpleGE::Game::RunResult::Success ? 0 : 1;
}