#pragma once

#include "chickendodge/pch/precomp.h"

namespace ChickenDodge
{
  // # Classe *BackgroundLoaderComponent*
  // Cette classe instancie des sprites à partir d'un fichier
  // de description. Ces sprites sont positionnés dans une grille,
  // mais peuvent elle-mêmes être de tailles diverses.
  class BackgroundLoaderComponent : public SimpleGE::Component
  {
  public:
    static constexpr auto Type = "BackgroundLoader";

    struct Entry
    {
      std::string spriteName;
      bool isAnimated;
      int frameSkip;
    };

    using EntryMap = std::unordered_map<char, Entry>;

    struct Description
    {
      std::string description;
      SimpleGE::ComponentReference<SimpleGE::SpriteSheetComponent> spriteSheet;
      float scale;
      EntryMap entryMap;
    };

    BackgroundLoaderComponent(SimpleGE::Entity& owner) : SimpleGE::Component(owner) {}

    SetupResult Setup(const nlohmann::json& descr) override;
    SetupResult Setup(const Description& descr);

  private:
    void SetupImpl(const Description& descr);
  };
} // namespace ChickenDodge