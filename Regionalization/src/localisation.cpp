#include <simplege/pch/precomp.h>

#include <simplege/simplege.h>

using json = nlohmann::json;

namespace SimpleGE
{
  void Localisation::Init(const LocaleFiles& locales, std::string_view lang)
  {
    auto curLocale = locales.find(std::string(lang));
    if (curLocale == locales.end())
    {
      curLocale = locales.begin();
      fmt::print(stderr, "Config pour {} introuvable, on va utiliser {} à la place.\n", lang, curLocale->first);
    }

    auto content = Resources::Get<TextAsset>(curLocale->second);
    Ensures(content != nullptr);
    json::parse(content->Value()).get_to(Instance().localeStrings);
  }

  [[nodiscard]] std::string Localisation::GetImpl(std::string_view key, const LocaleContext& queryContext) const
  {
    LocaleContext mergedContext;
    std::set_union(queryContext.begin(), queryContext.end(), globalContext.begin(), globalContext.end(),
                   std::inserter(mergedContext, mergedContext.end()));

    auto localized = localeStrings.find(std::string(key));
    if (localized == localeStrings.end())
    {
      fmt::print(stderr, "Clé régionalisée {} introuvable.\n", key);
      return std::string(key);
    }

    // ***TODO***: Implémenter la substitution de clés
#define DUMP_MERGED_CONTEXT // Enlever quand ça fonctionne correctement
#ifdef DUMP_MERGED_CONTEXT
    // Pour éviter de spam les logs, n'afficher le dump qu'une fois
    static std::unordered_set<std::string> debugContextHistory;
    std::string debugMergedContext = json(mergedContext).dump();
    if (!debugContextHistory.contains(debugMergedContext))
    {
      debugContextHistory.insert(debugMergedContext);
      fmt::print("mergedContext: {}\n", debugMergedContext);
    }
#endif // DUMP_MERGED_CONTEXT

    return localized->second;
  }
} // namespace SimpleGE