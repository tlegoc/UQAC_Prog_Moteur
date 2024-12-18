#pragma once

#include <simplege/pch/precomp.h>

#include <simplege/resources.h>

namespace SimpleGE
{
  using LocaleContext = std::unordered_map<std::string, std::string>;
  using LocaleData = std::unordered_map<std::string, std::string>;
  using LocaleFiles = std::unordered_map<std::string, std::string>;

  // # Classe *Localisation*
  // Cette classe comprend les méthodes nécessaires pour
  // charger et traiter la régionalisation.
  class Localisation
  {
  public:
    [[nodiscard]] static Localisation& Instance()
    {
      static Localisation instance;
      return instance;
    }

    // ## Méthode statique *Init*
    // La méthode d'initialisation prend en paramètre un tableau
    // associatif décrivant les différents fichiers de localisation.
    static void Init(const LocaleFiles& locales, std::string_view lang);

    // ## Méthode statique *SetContext*
    // Cette méthode assigne une valeur au contexte
    // global qui sera substituée par défaut.
    static void SetContext(std::string_view key, std::string_view val) { Instance().SetContextImpl(key, val); }

    // ## Méthode statique *GetContext*
    // Cette méthode obtient une valeur du contexte global.
    [[nodiscard]] static const std::string& GetContext(std::string_view key) { return Instance().GetContextImpl(key); }

    // ## Fonction statique *Get*
    // Cette fonction retourne la chaîne correspondante à la clé demandée.
    // Si cette chaîne comprend des champs substitués, ceux-ci sont remplacés.
    [[nodiscard]] static std::string Get(std::string_view key, const LocaleContext& queryContext = {})
    {
      return Instance().GetImpl(key, queryContext);
    }

    Localisation(const Localisation&) = delete;
    Localisation(Localisation&&) = delete;
    Localisation& operator=(const Localisation&) = delete;
    Localisation& operator=(Localisation&&) = delete;
    ~Localisation() = default;

  private:
    Localisation() = default;

    void SetContextImpl(std::string_view key, std::string_view val)
    {
      globalContext.insert_or_assign(std::string(key), std::string(val));
    }

    [[nodiscard]] const std::string& GetContextImpl(std::string_view key) { return globalContext.at(std::string(key)); }

    [[nodiscard]] std::string GetImpl(std::string_view key, const LocaleContext& queryContext) const;

    LocaleContext globalContext;
    LocaleData localeStrings;
  };
} // namespace SimpleGE